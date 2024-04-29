//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <nlohmann/json.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <NESInteraction/NESRequest.h>
#include <NESInteraction/NESSimLoad.h>
#include <Vector/Vec3D.h>
#include <Util/JSONUtils.h>
#include <PCRegistration/SimpleRegistration.h>
#include <Validation/DataCollector.h>

namespace BG {

bool NetworkData::EnsureLoaded(SafeClient & _Client, const ValidationConfig & _Config) {
	if (Loaded) return true;

	Loaded = AwaitNESSimLoad(_Client, SaveName, SimID, _Config.Timeout_ms);
	return Loaded;
}

bool NetworkData::EnsureGotSomaPositions(SafeClient & _Client, const ValidationConfig & _Config) {
	if (SomaPositionsLoaded) return true;

	if (!EnsureLoaded(_Client, _Config)) return false;

	nlohmann::json Response;
	if (!MakeNESRequest(_Client, "Simulation/GetSomaPositions", nlohmann::json::parse("{ \"SimulationID\": "+std::to_string(SimID)+" }"), Response)) {
		return false;
	}
	nlohmann::json& FirstResponse = Response[0];
	if (GetParVecVec3D(*_Client.Logger_, FirstResponse, "SomaCenters", SomaCenters) != BGStatusCode::BGStatusSuccess) {
		return false;
	}
	if (GetParIntVec(*_Client.Logger_, FirstResponse, "SomaTypes", SomaTypes) != BGStatusCode::BGStatusSuccess) {
		return false;
	}
	SomaPositionsLoaded = true;
	return true;
}

bool NetworkData::EnsureGotConnections(SafeClient & _Client, const ValidationConfig & _Config) {
	if (ConnectionsLoaded) return true;

	if (!EnsureLoaded(_Client, _Config)) return false;

	nlohmann::json Response;
	if (!MakeNESRequest(_Client, "Simulation/GetConnectome", nlohmann::json::parse("{ \"SimulationID\": "+std::to_string(SimID)+" }"), Response)) {
		return false;
	}
	nlohmann::json& FirstResponse = Response[0];
	if (GetParVecIntVec(*_Client.Logger_, FirstResponse, "ConnectionTargets", ConnectionTargets) != BGStatusCode::BGStatusSuccess) {
		return false;
	}
	if (GetParVecIntVec(*_Client.Logger_, FirstResponse, "ConnectionTypes", ConnectionTypes) != BGStatusCode::BGStatusSuccess) {
		return false;
	}
	if (GetParVecFloatVec(*_Client.Logger_, FirstResponse, "ConnectionWeights", ConnectionWeights) != BGStatusCode::BGStatusSuccess) {
		return false;
	}
	ConnectionsLoaded = true;
	return true;
}

bool NetworkData::EnsureCentered(SafeClient & _Client, const ValidationConfig & _Config) {
	if (NetworkCentered) return true;

	if (!EnsureGotSomaPositions(_Client, _Config)) return false;

	if (!CentroidCalculated) {
		centroid = GeometricCenter(SomaCenters);
		CentroidCalculated = true;
	}

	if (!NetworkCentered) {
		for (auto & v : SomaCenters) {
			v -= centroid;
		}
		NetworkCentered = true;
	}
	return true;
}

/**
 * Creates the flat Connectome info map for use in validation.
 * Also creates an Emu2KGT equivalent of KGT2Emu for mapping between networks.
 */
bool NetworkData::EnsureConnectome(SafeClient & _Client, const ValidationConfig & _Config, const std::vector<int>& KGT2Emu, std::map<int, int>& Emu2KGT, size_t _NumVertices) {
	if (BuiltConnectome) return true;

	if (!EnsureGotConnections(_Client, _Config)) return false;

	NumVertices = ConnectionTargets.size();
	_Connectome.Vertices.resize(NumVertices);


	for (size_t i = 0; i < KGT2Emu.size(); i++) {
		_Connectome.Vertices[i] = std::make_unique<Vertex>(VertexType(SomaTypes[i]));
	}

	// Create a vertex for each  euron and create its edges based on connections.
	for (size_t i = 0; i < KGT2Emu.size(); i++) {
		// From neuron i to Vertex i, add connections.
		for (size_t j = 0; j < ConnectionTargets[i].size(); j++) {
			int target_id = ConnectionTargets[i][j];
			int source_id = i;

			assert(source_id >= 0 && source_id < NumVertices && "SourceID Not Valid!");
			assert(target_id >= 0 && target_id < NumVertices && "TargetID Not Valid!");

			EdgeType type_ = EdgeType(ConnectionTypes[i][j]);
			float weight_ = ConnectionWeights[i][j];
			// *** TODO: With all the allocations involved this is probably unnecessarily slow.
			_Connectome.Vertices[source_id]->OutEdges[target_id] = std::make_unique<Edge>(type_, weight_);
			_Connectome.Vertices[target_id]->InEdges[source_id] = std::make_unique<Edge>(type_, weight_);
		}
	}
	
	if (!IsKGT) {
		// Make a reverse conversion map.
		for (size_t i = 0; i < SomaCenters.size(); i++) Emu2KGT.emplace(i, -1);
		for (size_t i = 0; i < KGT2Emu.size(); i++) if (KGT2Emu[i] >= 0) Emu2KGT[KGT2Emu[i]] = i;
	}

	BuiltConnectome = true;
	return true;
}

size_t NetworkData::GetConnectomeTotalElements() const {
	size_t elementcount = 0;
	for (const auto & vert : _Connectome.Vertices) {
		if (vert) {
			elementcount++; // For the vertex.
			elementcount += vert->OutEdges.size(); // For the edges.
		}
	}
	return elementcount;
}

nlohmann::json NetworkData::GetConnectomeJSON() const {
	nlohmann::json ConnectomeJSON(nlohmann::json::value_t::array);

	for (size_t vertex_id = 0; vertex_id < _Connectome.Vertices.size(); vertex_id++) {
		ConnectomeJSON[vertex_id] = nlohmann::json::object();
		if (_Connectome.Vertices.at(vertex_id)) {
			ConnectomeJSON[vertex_id]["Type"] = int(_Connectome.Vertices.at(vertex_id)->type_); //VertexType2Label[_Connectome.Vertices.at(vertex_id)->type_];
			ConnectomeJSON[vertex_id]["Edges"] = nlohmann::json::array();
			nlohmann::json& edges = ConnectomeJSON[vertex_id]["Edges"];
			for (const auto& [ target_id, e ] : _Connectome.Vertices.at(vertex_id)->OutEdges) {
				if (e) {
					edges.push_back(nlohmann::json::array({ target_id, int(e->type_), e->weight_ }));
				}
			}
		}
	}

	return ConnectomeJSON;
}


nlohmann::json GraphEdit::GetJSON() const {
	nlohmann::json GEJSON;
	GEJSON["Op"] = int(Op);
	GEJSON["Data"] = Element;
	GEJSON["Cost"] = cost;
	return GEJSON;
}


DataCollector::DataCollector(const std::string& _KGTSaveName, const std::string& _EMUSaveName):
	KGTData(true), EMUData(false) {
	KGTData.SaveName = _KGTSaveName;
	EMUData.SaveName = _EMUSaveName;
}

bool DataCollector::EnsureLoaded(SafeClient & _Client, const ValidationConfig & _Config) {
	return KGTData.EnsureLoaded(_Client, _Config) && EMUData.EnsureLoaded(_Client, _Config);
}

bool DataCollector::EnsureGotSomaPositions(SafeClient & _Client, const ValidationConfig & _Config) {
	return KGTData.EnsureGotSomaPositions(_Client, _Config) && EMUData.EnsureGotSomaPositions(_Client, _Config);
}

bool DataCollector::EnsureCentered(SafeClient & _Client, const ValidationConfig & _Config) {
	return KGTData.EnsureCentered(_Client, _Config) && EMUData.EnsureCentered(_Client, _Config);
}

bool DataCollector::EnsureRegistered(SafeClient & _Client, const ValidationConfig & _Config) {
	if (Registered) return true;

	Registered = SimpleRegistration(_Client, _Config, *this);
	return Registered;
}

bool DataCollector::EnsureConnectomes(SafeClient & _Client, const ValidationConfig & _Config) {
	if (Connectomes) return true;

	if (!EnsureRegistered(_Client, _Config)) return false;

	size_t NumVertices = KGTData.SomaCenters.size() > EMUData.SomaCenters.size() ? KGTData.SomaCenters.size() : EMUData.SomaCenters.size();
	Connectomes = KGTData.EnsureConnectome(_Client, _Config, KGT2Emu, Emu2KGT, NumVertices) && EMUData.EnsureConnectome(_Client, _Config, KGT2Emu, Emu2KGT, NumVertices);
	return Connectomes;
}

nlohmann::json DataCollector::GetConnectomesJSON() const {
	nlohmann::json ConnectomesJSON;

	ConnectomesJSON["KGTConnectome"] = KGTData.GetConnectomeJSON();
	ConnectomesJSON["EMUConnectome"] = EMUData.GetConnectomeJSON();

	return ConnectomesJSON;
}

nlohmann::json DataCollector::GetKGT2EmuMapJSON() const {
	nlohmann::json KGT2EmuJSON(nlohmann::json::value_t::array);

	for (const auto& Emu_i : KGT2Emu) {
		KGT2EmuJSON.push_back(Emu_i);
	}

	return KGT2EmuJSON;
}

nlohmann::json DataCollector::GetGraphEditsJSON() const {
	nlohmann::json GraphEditsJSON(nlohmann::json::value_t::array);

	for (const auto& ge : N1Metrics.GraphEdits) {
		GraphEditsJSON.push_back(ge.GetJSON());
	}

	return GraphEditsJSON;
}

nlohmann::json DataCollector::GetScoresJSON() const {
	nlohmann::json ScoresJSON;

	ScoresJSON["GraphEditRawCost"] = N1Metrics.GED_total_cost_raw;
	ScoresJSON["NumElements"] = N1Metrics.KGT_elements_total;
	ScoresJSON["GraphEditScore"] = N1Metrics.GED_score;

	return ScoresJSON;
}

} // BG
