//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Util/JSONUtils.h>
#include <Validation/ValidationTestData.h>

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

    		float t_fire = pair_json[0].template get<float>();
    		int neuron_id = pair_json[1].template get<int>();

			if (neuron_id < 0) {

				MaxRecordTime_ms = t_fire;

			} else {

				KGT_t_soma_fire_ms.emplace_back(neuron_id, t_fire);

			}

    	}

    	IsValid_ = true;

    } else { // Not one of the recognized test data formats
        Logger_.Log("Error Test Data format unrecognized. Request Is: " + _RequestJSON.dump(), 7);
    }
}

ValidationTestData::ValidationTestData(const ValidationTestData& _Source, const std::vector<int>& Source2This): Logger_(_Source.Logger_), MaxRecordTime_ms(_Source.MaxRecordTime_ms) {
	for (const auto& SomaIDTfirePair : _Source.KGT_t_soma_fire_ms) {

		if (SomaIDTfirePair.SomaID >= Source2This.size()) {
			Logger_.Log("Validation test data refers to KGT neuron that is not in the KGT2Emu map: "+std::to_string(SomaIDTfirePair.SomaID), 7);
			return;
		}
		if (Source2This.at(SomaIDTfirePair.SomaID) < 0) {
			Logger_.Log("Warning, skipping test input for KGT neruon that does not appear in EMU: "+std::to_string(SomaIDTfirePair.SomaID), 5);
			continue;
		}
		KGT_t_soma_fire_ms.emplace_back(Source2This.at(SomaIDTfirePair.SomaID), SomaIDTfirePair.t_fire);

	}

	IsValid_ = true;
}

nlohmann::json ValidationTestData::GetSomaAPTimes() const {
	nlohmann::json SomaIDTFirePairsListJSON(nlohmann::json::value_t::array);
    for (const auto& somafirepair : KGT_t_soma_fire_ms) {
        SomaIDTFirePairsListJSON.push_back(nlohmann::json::parse("["+std::to_string(somafirepair.SomaID)+','+std::to_string(somafirepair.t_fire)+"]"));
    }
	return SomaIDTFirePairsListJSON;
}

} // BG
