//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <rpc/rpc_error.h>

// Internal Libraries (BG convention: use <> instead of "")

#include <RPC/SafeClient.h>

#include <RPC/RPCHandlerHelper.h>
#include <RPC/APIStatusCode.h>

#include <Version.h>


namespace BG {
namespace EVM {


bool SafeClient::RunVersionCheck() {

    // Run a query to force it to connect (or fail)
    std::string Temp;
    MakeJSONQuery("GetAPIVersion", &Temp);
    

    // Update our internal status of how the connection is doing
    ::rpc::client::connection_state NESStatus = Client_->get_connection_state();
    if (NESStatus != ::rpc::client::connection_state::connected) {
        Logger_->Log("Unable to connect to NES service", 3);
    } else {
        Logger_->Log("NES RPC Connection SERVICE_HEALTHY", 1);
    }

    // Check Version again (used as a heartbeat 'isAlive' check)
    std::string NESVersion = "undefined";
    bool Status = MakeJSONQuery("GetAPIVersion", &NESVersion, true);
    if (!Status) {
        Logger_->Log("Failed To Get NES API Version String", 1);
        return false;
    }

    if (NESVersion != VERSION) {
        Logger_->Log("Client/Server Version Mismatch! This might make stuff break. Server " + NESVersion + " Client " + VERSION, 9);
        return false;
    }
    return true;


}

bool SafeClient::Connect() {
    IsHealthy_ = false;
    Client_ = nullptr;

    // Extract NES Client Parameters, Connect, Configure
    std::string NESHost = RPCHost_;
    int NESPort = RPCPort_;
    int NESTimeout_ms = RPCTimeout_ms;
    
    Logger_->Log("Connecting to NES on port: " + std::to_string(NESPort), 1);
    Logger_->Log("Connecting to NES on host: " + NESHost, 1);
    Logger_->Log("Connecting to NES with timeout_ms of: " + std::to_string(NESTimeout_ms), 1);


    try {
        Client_ = std::make_unique<::rpc::client>(NESHost.c_str(), NESPort);
    } catch (std::system_error& e) {
        Logger_->Log("Cannot find NES host (authoritative)", 9);
        return false;
    }
    Client_->set_timeout(NESTimeout_ms);

    // Call GetVersion On Remote - allows us to check that versions match, but also ensures the connection is ready
    bool Status = RunVersionCheck();
    if (Status) {
        IsHealthy_ = true;
    }
    return Status;

}


SafeClient::SafeClient(BG::Common::Logger::LoggingSystem* _Logger) {

    Logger_ = _Logger;
    RequestExit_ = false;
    ClientManager_ = std::thread(&SafeClient::RPCManagerThread, this);

}

SafeClient::~SafeClient() {
    RequestExit_ = true;
    ClientManager_.join();
}

bool SafeClient::SetTimeout(int _Timeout_ms) {
    RPCTimeout_ms = _Timeout_ms;
    return true;
}
bool SafeClient::SetHostPort(std::string _Host, int _Port) {
    RPCHost_ = _Host;
    RPCPort_ = _Port;
    return true;
}

void SafeClient::Reconnect() {
    IsHealthy_ = false;
}

bool SafeClient::MakeJSONQuery(std::string _Route, std::string* _Result, bool _ForceQuery) {
    if (!_ForceQuery && !IsHealthy_) {
        return false;
    }
    try {
        (*_Result) = Client_->call(_Route.c_str()).as<std::string>();
    } catch (::rpc::timeout& e) {
        Logger_->Log("RPC Connection Timed Out", 3);
        return false;
    } catch (::rpc::rpc_error& e) {
        Logger_->Log("RPC Remote Returned Error", 3);
        return false;
    } catch (std::system_error& e) {
        Logger_->Log("Cannot Cannot Talk To RPC Host", 3);
        return false;
    }
    return true;
}


bool SafeClient::MakeJSONQuery(std::string _Route, std::string _Query, std::string* _Result, bool _ForceQuery) {
    if (!_ForceQuery && !IsHealthy_) {
        return false;
    }
    try {
        (*_Result) = Client_->call(_Route.c_str(), _Query).as<std::string>();
    } catch (::rpc::timeout& e) {
        Logger_->Log("NES Connection timed out!",3);
        return false;
    } catch (::rpc::rpc_error& e) {
        Logger_->Log("NES remote returned RPC error",3);
        return false;
    } catch (std::system_error& e) {
        Logger_->Log("Cannot talk to NES host",3);
        return false;
    }
    return true;
}


void SafeClient::RPCManagerThread() {

    // Wait Until Config Valid
    while (RPCHost_ == "") {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }


    // Enter loop
    while (!RequestExit_) {

        // Check Version
        bool IsHealthy = RunVersionCheck();

        // If not healthy, re-establish connection, retry stuff... For now, nothing...
        if (!IsHealthy) {
            if (!Connect()) {
                Logger_->Log("Failed To Reconnect To NES Service",3);
            }
        }

        // Wait 1000ms before polling again
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));


    }
}

}; // Close Namespace EVM
}; // Close Namespace BG