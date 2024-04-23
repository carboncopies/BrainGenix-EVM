//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <NESInteraction/NESSimLoad.h>
#include <PCRegistration/SimpleRegistration.h>
#include <Validation/SCValidation.h>

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

	Logger_->Log("Commencing validation of Simple Compartmental ground-truth and emulation systems.",1);

	// Load the specified ground-truth system.
	int KGTSimID;
	if (!AwaitNESSimLoad(_Client, _KGTSaveName, KGTSimID, Config.Timeout_ms)) {
		return false;
	}

	// Load the specified emulation system.
	int EmuSimID;
	if (!AwaitNESSimLoad(_Client, _EmuSaveName, EmuSimID, Config.Timeout_ms)) {
		return false;
	}

	// Get a registration mapping from neurons in ground-truth to emulation.
	std::vector<int> KGT2Emu;
	if (!SimpleRegistration(_Client, KGTSimID, EmuSimID, KGT2Emu)) {
		return false;
	}

	// Apply the N1 success-criteria metrics
	N1Metrics N1Metrics_(_Client, KGTSimID, EmuSimID, KGT2Emu);
	if (!N1Metrics_.Validate()) {
		return false;
	}

	// *** There need to be more metrics applied here!

	return true;
}

} // BG
