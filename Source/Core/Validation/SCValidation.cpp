//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <PCRegistration/SimpleRegistration.h>
#include <Validation/DataCollector.h>
#include <Validation/ValidationConfig.h>
#include <Validation/SCValidation.h>
#include <Metrics/N1Metrics.h>

namespace BG {

/**
 * This is a simple entry point through which to carry out validation
 * using metrics that area suitable for a pair of emulation and
 * ground-truth systems expressed using Simple Compartmental neurons.
 * 
 * @param _Client
 * @param _KGTSaveName Known Ground-Truth system save name.
 * @param _EmuSaveName Emulation system save name.
 * @param _Config Configuration settings used.
 * @return True if successfully carried out.
 */
bool SCVAlidate(SafeClient & _Client, const std::string & _KGTSaveName, const std::string & _EmuSaveName, const ValidationConfig & _Config) {

	_Client.Logger_->Log("Commencing validation of Simple Compartmental ground-truth and emulation systems.",1);

	DataCollector CollectedData(_KGTSaveName, _EmuSaveName);

	// Apply the N1 success-criteria metrics
	N1Metrics N1Metrics_(_Client, _Config, CollectedData);
	if (!N1Metrics_.Validate()) {
		return false;
	}

	// *** There need to be more metrics applied here!

	return true;
}

} // BG
