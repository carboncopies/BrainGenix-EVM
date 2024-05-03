//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>
#include <chrono>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

// Internal Libraries (BG convention: use <> instead of "")
//#include <BGStatusCode.h>
#include <NESInteraction/NESRequest.h>
#include <Util/JSONUtils.h>
#include <NESInteraction/NESRunSimulation.h>

namespace BG {

bool RecordAll(SafeClient& _Client, int _SimID, float _MaxRecordTime_ms) {

    nlohmann::json Data;
    Data["SimulationID"] = _SimID;
    Data["MaxRecordTime_ms"] = _MaxRecordTime_ms;

    return MakeNESRequest(_Client, "Simulation/RecordAll", Data);
}

bool SetSpecificAPTimes(SafeClient& _Client, int _SimID, const std::vector<SomaAPTime_ms>& _SomaIDTFirePairsList) {

    nlohmann::json Data;
    Data["SimulationID"] = _SimID;
    Data["TimeNeuronPairs"] = _SomaIDTFirePairsList.GetSomaAPTimes();

    return MakeNESRequest(_Client, "Simulation/SetSpecificAPTimes", Data));
}

bool GetSimulationStatus(SafeClient& _Client, int _SimID, nlohmann::json& _ResponseJSON) {

    nlohmann::json Data;
    Data["SimulationID"] = _SimID;

    return MakeNESRequest(_Client, "Simulation/GetStatus", Data, _ResponseJSON);
}

bool GetSimulationStatus(SafeClient& _Client, int _SimID, float& _InSimulationTime_ms, bool& IsSimulating) {
    nlohmann::json SimStatusJSON;
    if (!GetSimulationStatus(_Client, _SimID, SimStatusJSON)) {
        return false;
    }

    nlohmann::json& SimStatusFirstResponse = SimStatusJSON[0];
    if (!GetParFloat(*_Client.Logger_, SimStatusFirstResponse, "InSimulationTime_ms", _InSimulationTime_ms)) {
        return false;
    }
    if (!GetParBool(*Client_.Logger_, SimStatusFirstResponse, "IsSimulating", IsSimulating)) {
        return false;
    }

    return true;
}

bool SimulationRunFor(SafeClient& _Client, int _SimID, float _SimulationDuration_ms) {

    nlohmann::json Data;
    Data["SimulationID"] = _SimID;
    Data["Runtime_ms"] = _SimulationDuration_ms;

    return MakeNESRequest(_Client, "Simulation/RunFor", Data);
}

bool AwaitSimulationRunFor(SafeClient& _Client, int _SimID, float _SimulationDuration_ms) {

    // Obtain simulation timer before this next run.
    float t_before_run = 0.0;
    bool issimulating_before_run = false;
    if (!GetSimulationStatus(_Client, _SimID, t_before_run, issimulating_before_run)) {
        return false;
    }

    // Start this run.
    if (!SimulationRunFor(_Client, _SimID, _SimulationDuration_ms)) {
        return false;
    }

    // Await completion.
    unsigned int dt_ms = 5;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(dt_ms));

        float t_ms = 0.0;
        bool issimulating = false;
        if (!GetSimulationStatus(_Client, _SimID, t_ms, issimulating)) {
            _Client.Logger_->Log("Simulation failed.", 7);
            return false;
        }
        if ((t_ms > t_before_run) && (!issimulating)) return true;
        // *** BEWARE: There is no timeout, so this can get stuck forever.
    }
    
}

bool GetRecording(SafeClient& _Client, int _SimID, nlohmann::json& _ResponseJSON) {

    nlohmann::json Data;
    Data["SimulationID"] = _SimID;

    return MakeNESRequest(_Client, "Simulation/GetRecording", Data, _ResponseJSON);

}

} // BG
