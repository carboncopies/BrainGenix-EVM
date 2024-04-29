//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Metrics/N1Metrics.h>
#include <Validation/Connectome.h>


namespace BG {

const std::map<GEDoperations, float> GEDOpCost = {
	{ vertex_insertion, 1.0 },
    { vertex_deletion, 1.0 },
    { vertex_substitution, 1.0 }, // Or 2.0?
    { edge_insertion, 1.0 }, // Or 0.5?
    { edge_deletion, 1.0 }, // Or 0.5?
    { edge_substitution, 1.0 }, // Or 0.75?
};

const std::map<VertexType, std::string> VertexType2Label = {
    { PrincipalNeuron, "PrincipalNeuron" },
    { Interneuron, "Interneuron" },
};

const std::map<EdgeType, std::string> EdgeType2Label = {
    { ExcitatoryConnection, "Excitatory" },
    { InhibitoryConnection, "Inhibitory" },
};

nlohmann::json GraphEdit::GetJSON() {
	nlohmann::json GEJSON;
	GEJSON["Op"] = int(Op);
	GEJSON["Data"] = Element;
	GEJSON["Cost"] = cost;
	return GEJSON;
}

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
 * 
 * The total score is given in two ways, namely:
 * 1. The raw GED cost obtained by summing the cost of all operations needed.
 * 2. A relative GED score that compares the total cost with the number of
 *    vertices and edges in the KGT that need to be deduced correctly.
 */
bool N1Metrics::PreRegisteredGED() {
	float total_GED_cost = 0.0;

	// 1. Find vertices in EMU that are not in KGT and delete them plus their connections.
	for (size_t Emu_i = 0; Emu_i < CollectedData.Emu2KGT.size(); Emu_i++) {
		if (CollectedData.Emu2KGT[Emu_i] < 0) { // Does not exist in KGT.
			// Delete outgoing edges.
			for (const auto & [target_id, edge] : CollectedData.EMUData._Connectome.Vertices.at(Emu_i)->OutEdges) {
				float cost = GEDOpCost.at(edge_deletion);
				GraphEdits.emplace_back(edge_deletion, std::to_string(Emu_i)+'>'+std::to_string(target_id), cost);
				total_GED_cost += cost;
			}
			// Delete incoming edges.
			for (const auto & [source_id, edge] : CollectedData.EMUData._Connectome.Vertices.at(Emu_i)->InEdges) {
				float cost = GEDOpCost.at(edge_deletion);
				GraphEdits.emplace_back(edge_deletion, std::to_string(source_id)+'>'+std::to_string(Emu_i), cost);
				total_GED_cost += cost;
			}
			// Delete vertex.
			float cost = GEDOpCost.at(vertex_deletion);
			GraphEdits.emplace_back(vertex_deletion, std::to_string(Emu_i), cost);
			total_GED_cost += cost;

		}
	}

	// 2. Find vertices in KGT that have no equivalent in EMU and insert them and their edges.
	for (size_t KGT_i = 0; KGT_i < CollectedData.KGT2Emu.size(); KGT_i++) {
		if (CollectedData.KGT2Emu[KGT_i] < 0) { // No equivalent in EMU.
			// Insert missing vertex.
			float cost = GEDOpCost.at(vertex_insertion);
			GraphEdits.emplace_back(vertex_insertion, "KGT"+std::to_string(KGT_i), cost);
			total_GED_cost += cost;
			// Insert incoming edges.
			for (const auto & [source_id, edge] : CollectedData.KGTData._Connectome.Vertices.at(KGT_i)->InEdges) {
				float cost = GEDOpCost.at(edge_insertion);
				GraphEdits.emplace_back(edge_insertion, "KGT"+std::to_string(source_id)+'>'+std::to_string(KGT_i), cost);
				total_GED_cost += cost;
			}
			// Insert outgoing edges.
			for (const auto & [target_id, edge] : CollectedData.KGTData._Connectome.Vertices.at(KGT_i)->OutEdges) {
				float cost = GEDOpCost.at(edge_insertion);
				GraphEdits.emplace_back(edge_insertion, "KGT"+std::to_string(KGT_i)+'>'+std::to_string(target_id), cost);
				total_GED_cost += cost;
			}

		}
	}

	// 3. For each vertex, check the type and switch it if necessary.
	for (size_t Emu_i = 0; Emu_i < CollectedData.Emu2KGT.size(); Emu_i++) {
		if (CollectedData.Emu2KGT[Emu_i] >= 0) {
			auto EmuVertexType = CollectedData.EMUData._Connectome.Vertices.at(Emu_i)->type_;
			auto KGTVertexType = CollectedData.KGTData._Connectome.Vertices.at(CollectedData.Emu2KGT[Emu_i])->type_;
			if (EmuVertexType != KGTVertexType) {
				float cost = GEDOpCost.at(vertex_substitution);
				auto it = VertexType2Label.find(KGTVertexType);
				if (it == VertexType2Label.end()) {
					Client_.Logger_->Log("KGT vertex type refers to unknown type: "+std::to_string(int(KGTVertexType)), 7);
					return false;
				}
				GraphEdits.emplace_back(vertex_substitution, it->second, cost);
				total_GED_cost += cost;
			}

			// Check the types of outgoing edges only (we don't want to double-count errors).
			for (const auto & [target_id, edge] : CollectedData.EMUData._Connectome.Vertices.at(Emu_i)->OutEdges) {
				// Find equivalent edge in the KGT.
				int KGT_source_id = CollectedData.Emu2KGT[Emu_i];
				int KGT_target_id = CollectedData.Emu2KGT[target_id];
				if ((KGT_source_id >= 0) && (KGT_target_id >= 0)) {
					auto EmuEdgeType = edge->type_;
					auto KGTEdgeType = CollectedData.KGTData._Connectome.Vertices.at(KGT_source_id)->OutEdges.at(KGT_target_id)->type_;
					// Edit graph notes and cost if error.
					if (EmuEdgeType != KGTEdgeType) {
						float cost = GEDOpCost.at(edge_substitution);
						auto it = EdgeType2Label.find(KGTEdgeType);
						if (it == EdgeType2Label.end()) {
							Client_.Logger_->Log("KGT edge type refers to unknown type: "+std::to_string(int(KGTEdgeType)), 7);
							return false;
						}
					}

				} else { // Edge that should not exist.
					float cost = GEDOpCost.at(edge_deletion);
					GraphEdits.emplace_back(edge_deletion, std::to_string(Emu_i)+'>'+std::to_string(target_id), cost);
					total_GED_cost += cost;
				}
			}
		}
	}

	// 4. For each vertex, check edges that are missing and insert them.
	//    This is exclusively for edges between two vertices that both exist in EMU. The
	//    cases where vertices were missed in EMU and all their edges were already dealt
	//    with in step 2.
	for (size_t Emu_i = 0; Emu_i < CollectedData.Emu2KGT.size(); Emu_i++) {
		int KGT_i = CollectedData.Emu2KGT[Emu_i];
		if (KGT_i >= 0) {
			// Check each intended outgoing edge
			for (const auto & [KGT_target_id, edge] : CollectedData.KGTData._Connectome.Vertices.at(KGT_i)->OutEdges) {
				int EMU_target_id = CollectedData.KGT2Emu[KGT_target_id];
				auto it = CollectedData.EMUData._Connectome.Vertices.at(Emu_i)->OutEdges.find(EMU_target_id);
				if (it == CollectedData.EMUData._Connectome.Vertices.at(Emu_i)->OutEdges.end()) { // Missing edge.
					float cost = GEDOpCost.at(edge_insertion);
					GraphEdits.emplace_back(edge_insertion, std::to_string(Emu_i)+'>'+std::to_string(EMU_target_id), cost);
					total_GED_cost += cost;
				}
			}
		}
	}

	// 5. Calculate the final GED score.
	GED_total_cost_raw = total_GED_cost;
	KGT_elements_total = CollectedData.KGTData.GetConnectomeTotalElements();
	GED_score = GED_total_cost_raw / float(KGT_elements_total);

	return true;
}

bool N1Metrics::ValidateAccurateSystemIdentification() {

	size_t KGT_n = CollectedData.KGTData.SomaCenters.size();
	size_t EMU_n = CollectedData.EMUData.SomaCenters.size();

	CollectedData.N1Metrics.num_neurons_absdiff = (KGT_n > EMU_n) ? KGT_n - EMU_n : EMU_n - KGT_n;
	CollectedData.N1Metrics.num_neurons_diff_pct = 100.0*float(CollectedData.N1Metrics.num_neurons_absdiff)/float(KGT_n);

	// Calculate the graph dit distance (GED).
	if (!PreRegisteredGED()) return false;

	// Calculate the Jaccard distance.
	// *** Not yet implemented.

	// Calculate the Quantum JSD.
	// *** Not yet implemented.

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
