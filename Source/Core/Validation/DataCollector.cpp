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
	if (!MakeNESRequest(_Client, "Simulation/GetSomaPositions", nlohmann::json("{ \"SimID\": "+std::to_string(SimID)+" }"), Response)) {
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
	if (!MakeNESRequest(_Client, "Simulation/GetConnectome", nlohmann::json("{ \"SimID\": "+std::to_string(SimID)+" }"), Response)) {
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
	// Create a vertex for each  euron and create its edges based on connections.
	for (size_t i = 0; i < KGT2Emu.size(); i++) {
		_Connectome.Vertices[i] = std::make_unique<Vertex>(VertexType(SomaTypes[i]));
		// From neuron i to Vertex i, add connections.
		for (size_t j = 0; j < ConnectionTargets[i].size(); j++) {
			int target_id = ConnectionTargets[i][j];
			int source_id = i;
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

} // BG
