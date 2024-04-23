//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <PCRegistration/SimpleRegistration.h>

namespace BG {

/**
 * Uses centering and rotations to find the best registration of one network
 * onto the other and returns a cell ID map in accordance with the registration
 * that can be used for subsequent validation processes.
 * 
 * Note that the two simulations must be loaded before calling this function
 * with their numerical IDs.
 * 
 * @param _Client Reference to NES-connected client object.
 * @param _SimIDA Identifier of saved system A (typically a ground-truth system).
 * @param _SimIDB Identifier of saved system B (typically an emulation system).
 * @param _RegistrationMap Vector of cell indices specifying which neuron in B maps
 *        to the vector index neuron in A.
 * @return True if successfully registered.
 */
bool SimpleRegistration(SafeClient & _Client, int _SimIDA, int _SimIDB, std::vector<int> & _RegistrationMap) {

	Logger_->Log("Simple Registration of one simulation network onto another", 1);

	// 1. Request the soma positions of SimIDA
	std::string SimIDASomasRequest("[{\"ReqID\":0,\"Simulation/GetSomaPositions\": { \"SimID\": "+std::to_string(_SimIDA)+" } }]");
	bool Status = _Client.MakeJSONQuery("Simulation/GetSomaPositions", SimIDASomasRequest, &Response);

	// 2. Request the soma positions of SimIDB
	std::string SimIDBSomasRequest("[{\"ReqID\":0,\"Simulation/GetSomaPositions\": { \"SimID\": "+std::to_string(_SimIDB)+" } }]");
	bool Status = _Client.MakeJSONQuery("Simulation/GetSomaPositions", SimIDASomasRequest, &Response);

	// 3. Center both networks.

	// 4. Check proximity score for various angle combinations to find best score.

	// 5. Return registered correspondence ID map.

}

}; // Close Namespace BG
