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
 * *** TODO: I think we need to abandon the co-registration of the connectomes.
 *           I think that instead we need to just build simple connectomes within the
 *           indexing scheme of the KGT or EMU network itself. Then, apply the maps
 *           in one direction or the other during comparison operations. Otherwise,
 *           ther is a high risk that some data will not be available.
 * As connectomes are built, they are built such that the vertex numbers are already co-registered,
 * meaning vertex 0 in KGT corresponds to vertex 1 in KGT. To do this, each network must know if it
 * is the KGT or the EMU, and the KGT2Emu registration information must be provided.
 */
bool NetworkData::EnsureConnectome(SafeClient & _Client, const ValidationConfig & _Config, const std::vector<int>& KGT2Emu, std::map<int, int>& Emu2KGT, size_t _NumVertices) {
	if (BuiltConnectome) return true;

	if (!EnsureGotConnections(_Client, _Config)) return false;

	NumVertices = _NumVertices;
	_Connectome.Vertices.resize(NumVertices);
	if (IsKGT) {
		// Create a vertex for each KGT neuron and create its edges based on connections.
		for (size_t i = 0; i < KGT2Emu.size(); i++) {
			_Connectome.Vertices[i] = std::make_unique<Vertex>(VertexType(SomaTypes[i]));
			// From KGT neuron i to Vertex i, add connections.
			//_Connectome.Vertices[i]->Edges.resize(ConnectionTargets[i].size()); -- Sparse option...
			_Connectome.Vertices[i]->Edges.resize(NumVertices);
			for (size_t j = 0; j < ConnectionTargets[i].size(); j++) {
				int target_id = ConnectionTargets[i][j];
				_Connectome.Vertices[i]->Edges[target_id] = std::make_unique<Edge>(EdgeType(ConnectionTypes[i][j]), ConnectionWeights[i][j]);
			}
		}
	} else {
		// Make a reverse conversion map.
		for (size_t i = 0; i < SomaCenters.size(); i++) Emu2KGT.emplace(i, -1);
		for (size_t i = 0; i < KGT2Emu.size(); i++) if (KGT2Emu[i] >= 0) Emu2KGT[KGT2Emu[i]] = i;
		// Create a vertex for each EMU neuron and create its edges based on connections.
		for (size_t Emu_i = 0; Emu_i < Emu2KGT.size(); Emu_i++) {
			int KGT_i = Emu2KGT[i];
			_Connectome.Vertices[KGT_i] = std::make_unique<Vertex>(VertexType(SomaTypes[Emu_i]));
			// From EMU neuron i to Vertex Emu2KGT[i], add connections using Emu2KGT target translations.
			_Connectome.Vertices[KGT_i]->Edges.resize(NumVertices);
			for (size_t Emu_j = 0; Emu_j < ConnectionTargets[Emu_i].size(); Emu_j++) {
				int target_id = Emu2KGT[ConnectionTargets[Emu_i][Emu_j]];
				_Connectome.Vertices[KGT_i]->Edges[target_id] = std::make_unique<Edge>(EdgeType(ConnectionTypes[Emu_i][Emu_j]), ConnectionWeights[Emu_i][Emu_j]);
			}
		}
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
