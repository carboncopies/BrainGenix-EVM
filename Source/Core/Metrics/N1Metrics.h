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

// Third-Party Libraries (BG convention: use <> instead of "")
//#include <nlohmann/json.hpp>


// Internal Libraries (BG convention: use <> instead of "")


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
    int KGTSimID_ = -1;
    int EmuSimID_ = -1;
    const std:vector<int>& KGT2Emu_;
public:
    N1Metrics(SafeClient & _Client, int _KGTSimID, int _EmuSimID, const std::vector<int>& _KGT2Emu):
        Client_(_Client), KGTSimID_(_KGTSimID), EmuSimID_(_EmuSimID), KGT2Emu_(_KGT2Emu) {}

    bool Validate();

};

} // BG
