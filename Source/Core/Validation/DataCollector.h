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
#include <Vector/Vec3D.h>
#include <Validation/Connectome.h>
#include <Validation/ValidationConfig.h>

namespace BG {

// These correspond to the SomaTypes obtained.
enum NeuronType: int {
    UnknownNeuron = 0,
    GenericPrincipalNeuron = 1,
    GenericInterneuron = 2,
    NUMNeuronType
};

struct NetworkData {
    std::string SaveName;
    bool IsKGT = false;

    bool Loaded = false;
    int SimID;

    bool SomaPositionsLoaded = false;
    std::vector<Vec3D> SomaCenters;
    std::vector<long> SomaTypes;

    bool CentroidCalculated = false;
    Vec3D centroid;

    bool NetworkCentered = false;

    bool ConnectionsLoaded = false;
    std::vector<std::vector<long>> ConnectionTargets;
    std::vector<std::vector<long>> ConnectionTypes;
    std::vector<std::vector<float>> ConnectionWeights;

    bool BuiltConnectome = false;
    size_t NumVertices = 0; // This is always the max of KGT and EMU number of neurons.
    Connectome _Connectome;

    NetworkData(bool _IsKGT): IsKGT(_IsKGT) {}

    bool EnsureLoaded(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureGotSomaPositions(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureGotConnections(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureCentered(SafeClient & _Client, const ValidationConfig & _Config);

    /**
     * As connectomes are built, they are built such that the vertex numbers are already co-registered,
     * meaning vertex 0 in KGT corresponds to vertex 1 in KGT. To do this, each network must know if it
     * is the KGT or the EMU, and the KGT2Emu registration information must be provided.
     */
    bool EnsureConnectome(SafeClient & _Client, const ValidationConfig & _Config, const std::vector<int>& KGT2Emu, size_t _NumVertices);

};

struct N1MetricsData {
    size_t num_neurons_absdiff = 0;
    float num_neurons_diff_pct = 0.0;
};

struct DataCollector {

    NetworkData KGTData;

    NetworkData EMUData;

    bool Registered = false;
    Vec3D BestRotationAngles;
    std::vector<int> KGT2Emu; // Values specify which index in EMU matches the indexed one in KGT, e.g. emu_idx = KGT2Emu[kgt_idx].

    bool Connectomes = false;

    N1MetricsData N1Metrics;

    DataCollector(const std::string& _KGTSaveName, const std::string& _EMUSaveName);

    bool EnsureLoaded(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureGotSomaPositions(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureCentered(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureRegistered(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureConnectomes(SafeClient & _Client, const ValidationConfig & _Config);

};

} // BG
