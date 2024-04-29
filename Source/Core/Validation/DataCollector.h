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
#include <nlohmann/json.hpp>


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
    size_t NumVertices = 0; // This relates to the connetome generated from the cells as identified in this network. Use KGT2Emu and Emu2KGT maps for comparisons.
    Connectome _Connectome;

    NetworkData(bool _IsKGT): IsKGT(_IsKGT) {}

    bool EnsureLoaded(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureGotSomaPositions(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureGotConnections(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureCentered(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureConnectome(SafeClient & _Client, const ValidationConfig & _Config, const std::vector<int>& KGT2Emu, std::map<int, int>& Emu2KGT, size_t _NumVertices);

    size_t GetConnectomeTotalElements() const;

    nlohmann::json GetConnectomeJSON() const;

};

enum GEDoperations: int {
    vertex_insertion,
    vertex_deletion,
    vertex_substitution,
    edge_insertion,
    edge_deletion,
    edge_substitution,
    NUMGEDoperations
};

struct GraphEdit {
    GEDoperations Op;
    std::string Element;
    float cost;

    GraphEdit(GEDoperations _Op, const std::string& _Element, float _Cost): Op(_Op), Element(_Element), cost(_Cost) {}

    nlohmann::json GetJSON() const;
};

struct N1MetricsData {
    size_t num_neurons_absdiff = 0;
    float num_neurons_diff_pct = 0.0;

    //! Graph edit details for report.
    std::vector<GraphEdit> GraphEdits;

    float GED_total_cost_raw = 0.0;
    size_t KGT_elements_total = 0;
    float GED_score = 0.0; // A smaller score (smaller distance) is better.
};

struct DataCollector {

    NetworkData KGTData;

    NetworkData EMUData;

    bool Registered = false;
    Vec3D BestRotationAngles;
    std::vector<int> KGT2Emu; // Values (when > 0) specify which index in EMU matches the indexed one in KGT, e.g. emu_idx = KGT2Emu[kgt_idx].

    bool Connectomes = false;
    std::map<int, int> Emu2KGT; // This is set during EnsureConnectomes.

    N1MetricsData N1Metrics;

    DataCollector(const std::string& _KGTSaveName, const std::string& _EMUSaveName);

    bool EnsureLoaded(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureGotSomaPositions(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureCentered(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureRegistered(SafeClient & _Client, const ValidationConfig & _Config);

    bool EnsureConnectomes(SafeClient & _Client, const ValidationConfig & _Config);

    nlohmann::json GetConnectomesJSON() const;

    nlohmann::json GetKGT2EmuMapJSON() const;

    nlohmann::json GetGraphEditsJSON() const;

    nlohmann::json GetScoresJSON() const;

};

} // BG
