//===========================================================//
// This file is part of the BrainGenix-EVM Validation System //
//===========================================================//

/*
    Description: This file provides N1 Metrics.
    Additional Notes: None
    Date Created: 2024-04-22
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")
//#include <nlohmann/json.hpp>


// Internal Libraries (BG convention: use <> instead of "")
#include <RPC/SafeClient.h>
#include <Validation/ValidationConfig.h>

namespace BG {

struct NetworkData {
    std::string SaveName;

    bool Loaded = false;
    int SimID;

    bool SomaPositionsLoaded = false;
    std::vector<Vec3D> SomaCenters;

    bool CentroidCalculated = false;
    Vec3D centroid;

    bool NetworkCentered = false;

    bool EnsureLoaded(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureGotSomaPositions(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureCentered(SafeClient & _Client, const ValidationConfig & _Config);

};

struct N1MetricsData {
    size_t num_neurons_absdiff = 0;
    float num_neurons_diff_pct = 0.0;
};

struct DataCollector {

    NeuronPositionData KGTData;

    NeuronPositionData EMUData;

    bool Registered = false;
    Vec3D BestRotationAngles;
    std::vector<int> KGT2Emu; // Values specify which index in EMU matches the indexed one in KGT, e.g. emu_idx = KGT2Emu[kgt_idx].

    N1MetricsData N1Metrics;

    DataCollector(const std::string& _KGTSaveName, const std::string& _EMUSaveName);

    bool EnsureLoaded(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureGotSomaPositions(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureRegistered(SafeClient & _Client, const ValidationConfig & _Config);

};

} // BG
