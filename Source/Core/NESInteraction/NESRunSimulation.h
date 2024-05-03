//===========================================================//
// This file is part of the BrainGenix-EVM Validation System //
//===========================================================//

/*
    Description: This file provides functions to carry out simulation
    running via NES.
    Additional Notes: None
    Date Created: 2024-05-03
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")
//#include <nlohmann/json.hpp>


// Internal Libraries (BG convention: use <> instead of "")
//#include <BGStatusCode.h>
#include <RPC/SafeClient.h>
#include <Validation/ValidationTestData.h>


namespace BG {

bool RecordAll(SafeClient & _Client, int _SimID, float _MaxRecordTime_ms);

bool SetSpecificAPTimes(SafeClient& _Client, int _SimID, const nlohmann::json& _SomaIDTFirePairsList);

bool GetSimulationStatus(SafeClient& _Client, int _SimID, nlohmann::json& _ResponseJSON);
bool GetSimulationStatus(SafeClient& _Client, int _SimID, float& _InSimulationTime_ms, bool& IsSimulating);

bool SimulationRunFor(SafeClient& _Client, int _SimID, float _SimulationDuration_ms);
bool AwaitSimulationRunFor(SafeClient& _Client, int _SimID, float _SimulationDuration_ms);

bool GetRecording(SafeClient& _Client, int _SimID, nlohmann::json& _ResponseJSON);

} // BG
