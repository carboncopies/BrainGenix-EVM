//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Metrics/N1Metrics.h>

namespace BG {

bool N1Metrics::ValidateAccurateSystemIdentification() {

	size_t KGT_n = CollectedData.KGTData.SomaCenters.size();
	size_t EMU_n = CollectedData.EMUData.SomaCenters.size();

	CollectedData.N1Metrics.num_neurons_absdiff = (KGT_n > EMU_n) ? KGT_n - EMU_n : EMU_n - KGT_n;
	CollectedData.N1Metrics.num_neurons_diff_pct = 100.0*float(CollectedData.N1Metrics.num_neurons_absdiff)/float(KGT_n);

	// Derive the connectome (connectivity matrix).

	// *** Build nx DiGraph.

	// As needed, convert this to two di-graphs, one for the KGT, one for the EMU.

	// Calculate the edit distance.

	// Calculate the Jaccard distance.

	// Calculate the Quantum JSD.

	return true;
}

bool N1Metrics::ValidateAccurateTuning() {

	return true;
}

bool N1Metrics::Validate() {

	if (!CollectedData.EnsureRegistered(_Client, _Config)) return false;

	ValidateAccurateSystemIdentification();

	ValidateAccurateTuning();

	// Generate report.

	return true;
}

} // BG
