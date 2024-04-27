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
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
//#include <nlohmann/json.hpp>


// Internal Libraries (BG convention: use <> instead of "")
#include <RPC/SafeClient.h>
#include <Validation/ValidationConfig.h>
#include <Validation/DataCollector.h>

namespace BG {

enum GEDoperations {
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
};

/**
 * Methods with which to apply validation metrics for the success criterion:
 * N-1 Reconstruction of neuronal circuits through system identification
 * and tuning of properties is sufficiently accurate.
 * For more information see the Success Criteria paper.
 * 
 */
class N1Metrics {
protected:
    SafeClient & Client_;
    const ValidationConfig & Config;

public:
    DataCollector& CollectedData;

    //! Graph edit details for report.
    std::vector<GraphEdit> GraphEdits;

public:
    N1Metrics(SafeClient& _Client, const ValidationConfig& _Config, DataCollector& _CollectedData): Client_(_Client), Config(_Config), CollectedData(_CollectedData) {}

    /**
     * This is an efficient, simplified implementation of the Graph Edit Distance
     * that depends on pre-registration of the two graphs being compared. Having
     * pre-registered the graphs, as is possible by knowing neuron locations,
     * calculating the number and types of edits required to convert the Emmulation
     * graph into one isomorphic with the Known Ground-Truth graph is relatively
     * easy and quick.
     * 
     * Before calling this, ensure that the connectome di-graph has been obtained
     * for both networks.
     */
    bool PreRegisteredGED();

    bool ValidateAccurateSystemIdentification();

    bool ValidateAccurateTuning();

    bool Validate();

};

} // BG
