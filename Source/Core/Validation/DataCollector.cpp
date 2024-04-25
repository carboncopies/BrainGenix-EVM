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
#include <PCRegistration/SimpleRegistration.h>
#include <Validation/DataCollector.h>

namespace BG {

bool NetworkData::EnsureLoaded(SafeClient & _Client, const ValidationConfig & _Config) {
	if (Loaded) return true;

	Loaded = AwaitNESSimLoad(_Client, SaveName, SimID, _Config.Timeout_ms);
	return Loaded;
}

bool NetworkData::EnsureGotSomaPositions(SafeClient & _Client, const ValidationConfig & _Config) {
	if (!EnsureLoaded(_Client, _Config)) return false;

	nlohmann::json Response;
	if (!MakeNESRequest(_Client, "Simulation/GetSomaPositions", nlohmann::json("{ \"SimID\": "+std::to_string(SimID)+" }"), Response)) {
		return false;
	}
	nlohmann::json& FirstResponse = Response[0];
	if (GetParVecVec3D(*_Client.Logger_, FirstResponse, "SomaCenters", SomaCenters) != BGStatusCode::BGStatusSuccess) {
		return false;
	}
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

DataCollector::DataCollector(const std::string& _KGTSaveName, const std::string& _EMUSaveName) {
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

} // BG
