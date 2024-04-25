//===========================================================//
// This file is part of the BrainGenix-EVM Validation System //
//===========================================================//

/*
    Description: This file provides N1 Metrics.
    Additional Notes: None
    Date Created: 2024-04-22
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
//#include <nlohmann/json.hpp>


// Internal Libraries (BG convention: use <> instead of "")
#include <RPC/SafeClient.h>
#include <Validation/ValidationConfig.h>
#include <Validation/DataCollector.h>

namespace BG {

/**
 * Methods with which to apply validation metrics for the success criterion:
 * N-1 Reconstruction of neuronal circuits through system identification
 * and tuning of properties is sufficiently accurate.
 * For more information see the Success Criteria paper.
 * 
 */
class N1Metrics {
protected:
    SafeClient & Client_;
    const ValidationConfig & Config;

public:
    DataCollector& CollectedData;

public:
    N1Metrics(SafeClient& _Client, const ValidationConfig& _Config, DataCollector& _CollectedData): Client_(_Client), Config(_Config), CollectedData(_CollectedData) {}

    bool ValidateAccurateSystemIdentification();

    bool ValidateAccurateTuning();

    bool Validate();

};

} // BG
