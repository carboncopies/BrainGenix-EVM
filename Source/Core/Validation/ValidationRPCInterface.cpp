#include <Validation/ValidationRPCInterface.h>
#include <RPC/APIStatusCode.h>


// Third-Party Libraries (BG convention: use <> instead of "")

#include <thread>
#include <mutex>

namespace BG {

ValidationRPCInterface::ValidationRPCInterface(BG::Common::Logger::LoggingSystem* _Logger, API::RPCManager* _RPCManager) {
    assert(_Logger != nullptr);
    assert(_RPCManager != nullptr);

    Logger_ = _Logger;

    // Register Callbacks
    _RPCManager->AddRoute("Validation/SCValidation", std::bind(&ValidationRPCInterface::SCValidation, this, std::placeholders::_1));

}

ValidationRPCInterface::~ValidationRPCInterface() {
}

std::string ValidationRPCInterface::SCValidation(std::string _JSONRequest) {
    
}

} // BG
