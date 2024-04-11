//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")

#include <RPC/RPCManager.h>

#include <RPC/RPCHandlerHelper.h>
#include <RPC/APIStatusCode.h>


namespace BG {
namespace EVM {
namespace API {


RPCManager::RPCManager(Config::Config* _Config, BG::Common::Logger::LoggingSystem* _Logger) {

    Logger_ = _Logger;

    // Initialize Server
    std::string ServerHost = _Config->Host;
    int ServerPort = _Config->PortNumber;

    // Create a unique pointer to the RPC server and initialize it with the provided port number
    RPCServer_ = std::make_unique<rpc::server>(ServerPort);

    APIClient_ = std::make_unique<SafeClient>(_Logger);

    // Register Basic Routes
    // Add predefined routes to the RPC server
    AddRoute("GetAPIVersion", _Logger, &GetAPIVersion);
    AddRoute("Echo", _Logger, &Echo);
    AddRoute("EVM", Logger_, [this](std::string RequestJSON){ return EVMRequest(RequestJSON);});
    AddRoute("SetCallback", Logger_, [this](std::string RequestJSON){ return SetupCallback(RequestJSON);});

    // Add EVM Routes
    AddRoute("Debug/Echo", std::bind(Echo, std::placeholders::_1));
    AddRoute("Debug/DoubleEcho", std::bind(&RPCManager::DoubleEcho, this, std::placeholders::_1));
    // AddRoute("Debug/Echo", std::bind(&RPCManager::Echo, this, std::placeholders::_1));
    

    int ThreadCount = std::thread::hardware_concurrency();
    _Logger->Log("Starting RPC Server With '" + std::to_string(ThreadCount) + "' Threads", 5);
    
    // Start the RPC server asynchronously with the specified thread count
    RPCServer_->async_run(ThreadCount);
}

RPCManager::~RPCManager() {
    // Destructor
    // No explicit cleanup needed as smart pointers manage the RPC server's memory
}



void RPCManager::AddRoute(std::string _RouteHandle, std::function<std::string(std::string _JSONRequest)> _Function) {
    Logger_->Log("Registering Callback For Route '" + _RouteHandle + "'", 4);
    RequestHandlers_.insert(std::pair<std::string, std::function<std::string(std::string _JSONRequest)>>(_RouteHandle, _Function));
    // RouteAndHandler Handler;
    // Handler.Route_ = _RouteHandle;
    // Handler.Handler_ = _Function;
    // AddRequestHandler(_RouteHandle, Handler);
}




std::string RPCManager::SetupCallback(std::string _JSONRequest) {

    nlohmann::json Params = nlohmann::json::parse(_JSONRequest);
    std::string Host = Params["CallbackHost"];
    int Port = Params["CallbackPort"];

    APIClient_->SetHostPort(Host, Port);
    APIClient_->SetTimeout(2500);
    APIClient_->Reconnect();

    Logger_->Log("System Callback To API Service Registered To '" + Host + ":" + std::to_string(Port) + "'", 5);

    return "{\"StatusCode\": 0}";
}


/**
 * This expects requests of the following format:
 * [
 *   {
 *     "ReqID": <request-id>,
 *     "AddBSNeuron": {
 *       "SimulationID": <SimID>, // Logically, this could belong outside AddBSNeuron, but we wish to reuse backward compatible functions. 
 *       "Name": <name>,
 *       "SomaID": <soma-id>,
 *       "AxonID": <axon-id>,
 *       <etc... all parameters>
 *     }
 *   },
 *   <more requests>
 * ]
 */
std::string RPCManager::EVMRequest(std::string _JSONRequest, int _SimulationIDOverride) { // Generic JSON-based EVM requests.

    // Parse Request
    //Logger_->Log(_JSONRequest, 3);
    API::HandlerData Handle(_JSONRequest, Logger_, "EVM");
    if (Handle.HasError()) {
        return Handle.ErrResponse();
    }

    if (!Handle.ReqJSON().is_array()) {
        Logger_->Log("Bad format. Must be array of requests.", 8);
        return Handle.ErrResponse(API::BGStatusCode::BGStatusInvalidParametersPassed);       
    }

    // Build Response
    nlohmann::json ResponseJSON = nlohmann::json::array(); // Create empty array for the list of responses.

    // For each request in the JSON list:
    for (const auto& req : Handle.ReqJSON()) {

        int ReqID = -1;
        //int SimulationID = -1;
        std::string ReqFunc;
        nlohmann::json ReqParams;
        nlohmann::json ReqResponseJSON;
        //std::string Response;

        // Get the mandatory components of a request:
        for (const auto& [req_key, req_value]: req.items()) {
            if (req_key == "ReqID") {
                ReqID = req_value.template get<int>();
            //} else if (req_key == "SimID") {
            //    SimulationID = req_value.template get<int>();
            } else {
                ReqFunc = req_key;
                ReqParams = req_value;
            }
        }
        // if (BadReqID(ReqID)) { // e.g. < highest request ID already handled
        //     ReqResponseJSON["ReqID"] = ReqID;
        //     ReqResponseJSON["StatusCode"] = 1; // bad request id
        // } else {

        // Typically would call a specific handler from here, but let's just keep parsing.
        auto it = RequestHandlers_.find(ReqFunc);
        if (it == RequestHandlers_.end()) {
            Logger_->Log("Error, No Handler Exists For Call " + ReqFunc, 7);
            ReqResponseJSON["ReqID"] = ReqID;
            ReqResponseJSON["StatusCode"] = 1; // unknown request *** TODO: use the right code
            //Response = ReqResponseJSON.dump();
        } else {
            if (!it->second) {
                ReqResponseJSON["ReqID"] = ReqID;
                Logger_->Log("Error, Handler Is Null For Call " + ReqFunc + ", Continuing Anyway", 7);
                // ReqResponseJSON["StatusCode"] = 1; // not a valid EVM request *** TODO: use the right code
            } else {
                Logger_->Log("DEBUG -> Got Request For '" + ReqFunc + "'", 0);
                if (_SimulationIDOverride != -1) {
                    ReqParams["SimulationID"] = _SimulationIDOverride;
                }
                std::string Response = it->second(ReqParams.dump()); // Calls the handler.
                // *** TODO: Either:
                //     a) Convert handlers to return nlohmann::json objects so that we
                //        can easily add ReqResponseJSON["ReqID"] = ReqID here, or,
                //     b) Convert Response back to a ReqResponseJSON here in order to
                //        add that... (This is more work, lower performance...)
                //     Right now, we do b) (sadly...)
                ReqResponseJSON = nlohmann::json::parse(Response);
                ReqResponseJSON["ReqID"] = ReqID;
            }
        }

        // }
        ResponseJSON.push_back(ReqResponseJSON);

    }

    std::string Response = ResponseJSON.dump();
    if (Response.length() < 1024) { 
        Logger_->Log("DEBUG --> Responding: " + Response, 0); // For DEBUG
    } else {
        Logger_->Log("DEBUG --> Response Omitted Due To Length", 0); // For DEBUG
    }
    // if (!IsLoadingSim()) {
    //     std::cout << "DEBUG ---> Responding: " << ResponseJSON.dump() << '\n'; std::cout.flush();
    // }

    return Handle.ResponseAndStoreRequest(ResponseJSON, false); // See comments at ResponseAndStoreRequest().
}



std::string RPCManager::DoubleEcho(std::string _Request) {
    Logger_->Log("Echoing '" + _Request + "' To NES", 1);


    std::string EchoRequest = "[{\"ReqID\":0,\"Echo\":" + _Request + "}]";

    std::string Response = "";
    bool Status = APIClient_->MakeJSONQuery("Echo", EchoRequest, &Response);
    // Response = "{" + Response + "}";

    if (!Status) {
        Logger_->Log("Error During Query To NES", 7);
        return "{\"StatusCode\":9999}";
    }

    Logger_->Log("Got Back Echo Reply '" + Response + "'", 1);
    return nlohmann::json::parse(Response)[0].dump();
}




}; // Close Namespace API
}; // Close Namespace EVM
}; // Close Namespace BG