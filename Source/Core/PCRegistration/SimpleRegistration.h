//===========================================================//
// This file is part of the BrainGenix-EVM Validation System //
//===========================================================//

/*
    Description: This file provides registration of neurons in pairs of systems.
    Additional Notes: None
    Date Created: 2024-04-22
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")
//#include <nlohmann/json.hpp>


// Internal Libraries (BG convention: use <> instead of "")


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
bool SimpleRegistration(SafeClient & _Client, int _SimIDA, int _SimIDB, std::vector<int> & _RegistrationMap);

} // BG
