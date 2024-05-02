//===========================================================//
// This file is part of the BrainGenix-EVM Validation System //
//===========================================================//

/*
    Description: This file provides a definition of validation test data.
    Additional Notes: None
    Date Created: 2024-05-02
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
//#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <BG/Common/Logger/Logger.h>

namespace BG {

struct SomaAPTime_ms {
    int SomaID;
    float t_fire;

    SomaAPTime_ms(int _SomaID, float _t_fire): SomaID(_SomaID), t_fire(_t_fire) {}
};

/**
 * A class that can support multiple types of validation test data
 * and recognizes the type, tests its validity, obtains it from
 * a source, for example JSON data provided by an RPC call, and
 * makes it available to validation tests.
 */
class ValidationTestData {
protected:

    BG::Common::Logger::LoggingSystem& Logger_;

    bool IsValid_ = false;

    std::vector<SomaAPTime_ms> KGT_t_soma_fire_ms;

public:

    //! Constructor that obtains test data from JSON provided by an RPC call.
    ValidationTestData(BG::Common::Logger::LoggingSystem& _Logger, const nlohmann::json& _RequestJSON, const std::string& _DataLabel);


    bool Valid() { return IsValid_; }

};

} // BG
