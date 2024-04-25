//===========================================================//
// This file is part of the BrainGenix-EVM Validation System //
//===========================================================//

/*
    Description: This file provides entry points for emulation validation.
    Additional Notes: None
    Date Created: 2024-04-22
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")
//#include <nlohmann/json.hpp>


// Internal Libraries (BG convention: use <> instead of "")


namespace BG {

struct ValidationConfig {
    unsigned long Timeout_ms = 100000;
    unsigned int TryAngles = 12;
};

/**
 * This is a simple entry point through which to carry out validation
 * using metrics that area suitable for a pair of emulation and
 * ground-truth systems expressed using Simple Compartmental neurons.
 * 
 * @param _Client
 * @param _KGTSaveName Known Ground-Truth system save name.
 * @param _EmuSaveName Emulation system save name.
 * @param _Config Configuration settings used.
 * @return True if successfully carried out.
 */
bool SCVAlidate(SafeClient & _Client, const std::string & _KGTSaveName, const std::string & _EmuSaveName, const ValidationConfig & _Config);

} // BG
