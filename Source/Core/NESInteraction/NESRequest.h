//===========================================================//
// This file is part of the BrainGenix-EVM Validation System //
//===========================================================//

/*
    Description: This file provides NES request support.
    Additional Notes: None
    Date Created: 2024-04-24
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <RPC/SafeClient.h>


namespace BG {

bool MakeNESRequest(SafeClient& _Client, const std::string& _Route, const nlohmann::json& _Data, nlohmann::json& _Result);

} // BG
