//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Metrics/N1Metrics.h>

namespace BG {

const std::map<GEDoperations, float> GEDOpCost = {
	{ vertex_insertion, 1.0 },
    { vertex_deletion, 1.0 },
    { vertex_substitution, 1.0 },
    { edge_insertion, 0.5 },
    { edge_deletion, 0.5 },
    { edge_substitution, 0.5 },
};

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
bool N1Metrics::PreRegisteredGED() {
	float total_GED_cost = 0.0;

	// 1. Find vertices in EMU that are not in KGT and delete them plus their connections.
	for (size_t Emu_i = 0; Emu_i < CollectedData.Emu2KGT.size(); Emu_i++) {
		if (Emu2KGT[Emu_i] < 0) { // Does not exist in KGT.
			float cost = GEDOpCost[vertex_deletion];
			GraphEdits.emplace_back(vertex_deletion, std::to_string(Emu_i), cost);
			total_GED_cost += cost;
		}
	}

	// 2. Find vertices in KGT that have no equivalent in EMU and insert them and their edges.
	for (size_t i = 0; i < CollectedData.EMUData._Connectome.Vertices.size(); i++) {
		//if (CollectedData.EMUData._Connectome.Vertices[i]) { // Exists in EMU.
			//if (!CollectedData.KGTData._Connectome.Vertices[i]) { // Does not exist in KGT.
				// *** TODO: Continue here...
				/*
				  Attach to report, a vertex deletion for the vertex in Emu
				  and colect the cost of that operation.
				 */
			//}
		//}
	}

	// 3. For each vertex, check the type and switch it if necessary.

	// 4. For each vertex, check edges that should not exist and delete them.

	// 5. For each vertex, check edges that are missing and insert them.

	// 6. Calculate the final GED score.
}

bool N1Metrics::ValidateAccurateSystemIdentification() {

	size_t KGT_n = CollectedData.KGTData.SomaCenters.size();
	size_t EMU_n = CollectedData.EMUData.SomaCenters.size();

	CollectedData.N1Metrics.num_neurons_absdiff = (KGT_n > EMU_n) ? KGT_n - EMU_n : EMU_n - KGT_n;
	CollectedData.N1Metrics.num_neurons_diff_pct = 100.0*float(CollectedData.N1Metrics.num_neurons_absdiff)/float(KGT_n);

	// Derive the connectome (connectivity matrix).

	// *** Build nx DiGraph.

	// As needed, convert this to two di-graphs, one for the KGT, one for the EMU.

	// Calculate the edit distance.

	if (!PreRegisteredGED()) return false;

	// Calculate the Jaccard distance.

	// Calculate the Quantum JSD.

	return true;
}

bool N1Metrics::ValidateAccurateTuning() {

	return true;
}

bool N1Metrics::Validate() {

	if (!CollectedData.EnsureRegistered(Client_, Config)) return false;

	if (!CollectedData.EnsureConnectomes(Client_, Config)) return false;

	ValidateAccurateSystemIdentification();

	ValidateAccurateTuning();

	// Generate report.

	return true;
}

} // BG
