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
#include <nlohmann/json.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <RPC/SafeClient.h>
#include <Validation/ValidationConfig.h>
#include <Validation/DataCollector.h>


namespace BG {

/**
 * A common validation class with many shared features but some specific
 * validation approaches that depend on the sophistication level of the
 * challenge applied.
 */
class Validation {
protected:
    SafeClient& Client_;
    const std::string& KGTSaveName_;
    const std::string& EmuSaveName_;
    const ValidationConfig& Config_;

public:
    DataCollector CollectedData_;
    nlohmann::json ReportJSON_;
    std::string ReportStr_;

public:
    /**
     * @param _Client
     * @param _KGTSaveName Known Ground-Truth system save name.
     * @param _EmuSaveName Emulation system save name.
     * @param _Config Configuration settings used.
     * @return True if successfully carried out.
     */
    Validation(SafeClient & _Client, const std::string & _KGTSaveName, const std::string & _EmuSaveName, const ValidationConfig & _Config);

    bool GenerateReport();

    /**
     * This is a simple entry point through which to carry out validation
     * using metrics that area suitable for a pair of emulation and
     * ground-truth systems expressed using Simple Compartmental neurons.
     */
    bool SCValidate();

    // ...other validate options...

};

} // BG
