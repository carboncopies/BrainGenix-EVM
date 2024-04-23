// Third-Party Libraries (BG convention: use <> instead of "")
#include <thread>
#include <mutex>

// Local libraries
#include <ValidationRPCInterface.h>
#include <RPC/APIStatusCode.h>
#include <JSONUtils.h>


namespace BG {

ValidationRPCInterface::ValidationRPCInterface(BG::Common::Logger::LoggingSystem* _Logger, API::RPCManager* _RPCManager) {
    assert(_Logger != nullptr);
    assert(_RPCManager != nullptr);

    Logger_ = _Logger;
    NESAPIClient_ = *(_RPCManager->APIClient_.get());

    // Register Callbacks
    _RPCManager->AddRoute("Validation/SCValidation", std::bind(&ValidationRPCInterface::SCValidation, this, std::placeholders::_1));

}

ValidationRPCInterface::~ValidationRPCInterface() {
}

std::string ValidationRPCInterface::SCValidation(std::string _JSONRequest) {
    nlohmann::json RequestJSON(_JSONRequest);

    // Obtain required parameters
    std::string KGTSaveName;
    if (GetParString(RequestJSON, "KGTSaveName", KGTSaveName) != BGStatusCode::BGStatusSuccess) {
        return ErrResponse(BGStatusCode::BGStatusInvalidParametersPassed, "Missing KGTSaveName parameter.");
    }
    std::string EmuSaveName;
    if (GetParString(RequestJSON, "EmuSaveName", EmuSaveName) != BGStatusCode::BGStatusSuccess) {
        return ErrResponse(BGStatusCode::BGStatusInvalidParametersPassed, "Missing EmuSaveName parameter.");
    }

    // Obtain optional (configuration) parameters
    ValidationConfig Config; // Build this with default settings.
    GetParInt(RequestJSON, "Timeout_ms", Config.Timeout_ms);

    if (!SCVAlidate(NESAPIClient, KGTSaveName, EmuSaveName, Config)) {
        return ErrResponse(BGStatusCode::BGStatusGeneralFailure, "SC Validation failed.");
    }

    return SuccessResponse();
}

} // BG
