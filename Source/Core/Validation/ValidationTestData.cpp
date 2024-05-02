//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Validation/ValidationTestdata.h>

namespace BG {

ValidationTestData::ValidationTestData(BG::Common::Logger::LoggingSystem& _Logger, const nlohmann::json& _RequestJSON, const std::string& _DataLabel): Logger_(_Logger) {

	// Find the data by _DataLabel.
	nlohmann::json::iterator it;
    if (FindPar(Logger_, _RequestJSON, _DataLabel, it) != BGStatusCode::BGStatusSuccess) return;

	// Extract valid test data.
    if (it.value().is_array()) { // This is probably a list of enforced neuron firing (time, neuron_id) pairs

    	nlohmann::json& t_soma_fire_ms_json(it.value());
    	for (const auto& pair_json : t_soma_fire_ms_json) {

    		if (!pair_json.is_array()) {
    			Logger_.Log("Error, expected pairs of [neuron_id, t_fire]. Request Is: " + _RequestJSON.dump(), 7);
    			return;
    		}

    		if (pair_json.size() < 2) {
    			Logger_.Log("Error, expected pairs of [neuron_id, t_fire]. Request Is: " + _RequestJSON.dump(), 7);
    			return;
    		}

    		if ((!pair_json[0].is_number()) || (!pair_json[1].is_number())) {
    			Logger_.Log("Error, expected pairs of [neuron_id, t_fire]. Request Is: " + _RequestJSON.dump(), 7);
    			return;
    		}

    		int neuron_id = pair_json[0].template get<int>();
    		float t_fire = pair_json[1].template get<float>();

    		KGT_t_soma_fire_ms.emplace_back(neuron_id, t_fire);

    	}

    	IsValid_ = true;

    } else { // Not one of the recognized test data formats
        Logger_.Log("Error Test Data format unrecognized. Request Is: " + _RequestJSON.dump(), 7);
    }
}

} // BG
