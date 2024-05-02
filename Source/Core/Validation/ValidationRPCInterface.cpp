// Third-Party Libraries (BG convention: use <> instead of "")
#include <thread>
#include <mutex>

// Local libraries
#include <Validation/ValidationRPCInterface.h>
#include <Validation/SCValidation.h>
#include <RPC/APIStatusCode.h>
#include <Util/JSONUtils.h>


namespace BG {

ValidationRPCInterface::ValidationRPCInterface(BG::Common::Logger::LoggingSystem& _Logger, EVM::API::RPCManager& _RPCManager):
    Logger_(_Logger), NESAPIClient_(*(_RPCManager.GetAPIClient())) {

    // Register Callbacks
    _RPCManager.AddRoute("Validation/SCValidation", std::bind(&ValidationRPCInterface::SCValidation, this, std::placeholders::_1));

}

ValidationRPCInterface::~ValidationRPCInterface() {
}

/**
 * Required parameters:
 *   "KGTSaveName": <string>
 *   "EmuSaveName": <string>
 *   "TestSimData": <JSON>
 * 
 * Optional parameters:
 *   "Timeout_ms": <unsigned long>
 *   "TryAngles": <unsigned int>
 */
std::string ValidationRPCInterface::SCValidation(std::string _JSONRequest) {
    nlohmann::json RequestJSON = nlohmann::json::parse(_JSONRequest);

    // Obtain required parameters
    std::string KGTSaveName;
    if (GetParString(Logger_, RequestJSON, "KGTSaveName", KGTSaveName) != BGStatusCode::BGStatusSuccess) {
        return ErrResponse(BGStatusCode::BGStatusInvalidParametersPassed, "Missing KGTSaveName parameter.");
    }
    std::string EmuSaveName;
    if (GetParString(Logger_, RequestJSON, "EmuSaveName", EmuSaveName) != BGStatusCode::BGStatusSuccess) {
        return ErrResponse(BGStatusCode::BGStatusInvalidParametersPassed, "Missing EmuSaveName parameter.");
    }

    // Obtain required functional test data
    ValidationTestData TestData_(Logger_, RequestJSON, "TestSimData");
    if (!Testdata_.Valid()) {
        return ErrResponse(BGStatusCode::BGStatusInvalidParametersPassed, "Invalid functional validation test data.")
    }

    // Obtain optional (configuration) parameters
    ValidationConfig Config; // Build this with default settings.
    long Timeout_ms;
    if (GetParInt(Logger_, RequestJSON, "Timeout_ms", Timeout_ms) == BGStatusCode::BGStatusSuccess) {
        Config.Timeout_ms = Timeout_ms;
    }
    long TryAngles;
    if (GetParInt(Logger_, RequestJSON, "TryAngles", TryAngles) == BGStatusCode::BGStatusSuccess) {
        Config.TryAngles = TryAngles;
    }

    Validation Validation_(NESAPIClient_, KGTSaveName, EmuSaveName, ValidationTestData, Config);
    if (!Validation_.SCValidate()) {
        return ErrResponse(BGStatusCode::BGStatusGeneralFailure, "SC Validation failed.");
    }

    return SuccessResponse(Validation_.ReportJSON_);
}

} // BG
