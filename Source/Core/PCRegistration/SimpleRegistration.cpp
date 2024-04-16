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

std::string SimpleRegistration(std::string _Request) {
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

}; // Close Namespace BG
