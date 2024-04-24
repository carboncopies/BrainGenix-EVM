//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <BGStatusCode.h>
#include <NESInteraction/NESRequest.h>
#include <NESInteraction/NESSimLoad.h>
#include <Util/JSONUtils.h>

namespace BG {

bool GetNESStatus(SafeClient & _Client, long _TaskID, BGStatusCode & _StatusCode, nlohmann::json& _ResultJSON) {

    if (!MakeNESRequest(_Client, "ManTaskStatus", nlohmann::json("{ \"TaskID\": "+std::to_string(_TaskID)+" }"), _ResultJSON)) {
        return false;
    }
    nlohmann::json& FirstResonse = _ResultJSON[0];

    long StatusCodeInt;
    if (!GetParInt(*_Client.Logger_, FirstResonse, "StatusCode", StatusCodeInt) != BGStatusCode::BGStatusSuccess) {
        return false;
    }
    _StatusCode = BGStatusCode(StatusCodeInt);
    return true;
}

bool AwaitNESOutcome(SafeClient & _Client, long _TaskID, nlohmann::json& _ResultJSON, unsigned long _Timeout_ms) {
    unsigned long Timeout_ms = _Timeout_ms;
    while (true) {
        BGStatusCode StatusCode;
        if (!GetNESStatus(_Client, _TaskID, StatusCode, _ResultJSON)) {
            _Client.Logger_->Log("NES Status request failed while waiting for process to complete", 7);
            return false;
        }

        if (StatusCode == BGStatusSuccess) {
            return true; // Process appears to be done.
        }

        if (StatusCode != BGStatusSimulationBusy) {
            _Client.Logger_->Log("NES Process status code returned error: "+std::to_string(static_cast<int>(StatusCode)), 7);
            return false;
        }

        Timeout_ms--;
        if (Timeout_ms==0) {
            _Client.Logger_->Log("Awaiting NES Process request timed out after "+std::to_string(_Timeout_ms)+" ms", 7);
            return false;
        }
        std::this_thread::sleep_for (std::chrono::milliseconds(1));
    }
}

/**
 * Ask NES to load a previously saved simulation and wait for loading
 * to complete or fail.
 * 
 * @param _Client Reference to client connection with NES.
 * @param _SimSaveName Name of the previously saved simulation.
 * @param _SimID Reference through which to return the simulation ID.
 * @param _Timeout_ms Timeout that ensures this function cannot become stuck forever (e.g. due to broken connection),
 * @return True if loading was successful.
 */
bool AwaitNESSimLoad(SafeClient & _Client, const std::string & _SimSaveName, int & _SimID, unsigned long _Timeout_ms) {

    _Client.Logger_->Log("Await NES Sim Load '" + _SimSaveName + "'", 1);

    // Start a simulation load request.
    nlohmann::json ResponseJSON;
    if (!MakeNESRequest(_Client, "Simulation/Load", nlohmann::json("{ \"SavedSimName\": "+_SimSaveName+" }"), ResponseJSON)) {
        return false;
    }
    nlohmann::json& FirstResponse = ResponseJSON[0];

    // Get TaskID.
    long TaskID = -1;
    if (GetParInt(*_Client.Logger_, FirstResponse, "TaskID", TaskID) != BGStatusCode::BGStatusSuccess) {
        _Client.Logger_->Log("Missing loading task ID.", 7);
        return false;
    }

    // Wait for status to indicate that loading completed or failed.
    nlohmann::json ResultJSON;
    if (!AwaitNESOutcome(_Client, TaskID, ResultJSON, _Timeout_ms)) {
        _Client.Logger_->Log("Awaiting completion of NES load request failed", 7);
        return false;
    }
    nlohmann::json& FirstResult = ResultJSON[0];

    // If successful then return the simulation ID.
    long SimID = -1;
    if (GetParInt(*_Client.Logger_, FirstResult, "SimulationID", SimID) != BGStatusCode::BGStatusSuccess) {
        return false;
    }
    _SimID = SimID;

    return true;
}

} // BG
