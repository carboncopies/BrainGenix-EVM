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
#include <RPC/SafeClient.h>


namespace BG {

/**
 * Proximity difference between points in set A and set B.
 * A smaller value is a better match.
 */
float ProximityError(const std::vector<Vec3D>& _PointsA, const std::vector<Vec3D>& _PointsB, std::vector<size_t>* _FromANearestInB = nullptr);

/**
 * Brute force testing of angles of rotation on all axes to find the best
 * match between two sets of points.
 */
Vec3D BruteForceBestMatchAngles(const std::vector<Vec3D> _PointsA, const std::vector<Vec3D> _PointsB, std::vector<size_t>* _FromANearestInB = nullptr, unsigned int try_angles = 12);

/**
 * Use this if there are more points in A than in B to ensure that only
 * one point in A is mapped to a point in B, determined by rotated nearest
 * match. Excess map entries are set to -1.
 */
void MakeRegistrationMapUnique(const std::vector<Vec3D>& SomaCentersA, const std::vector<Vec3D>& SomaCentersB, const Vec3D& BestRotationAngles, std::vector<int> & _RegistrationMap);

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
 * @param _TryAngles Number of angles to try on each axis.
 * @return True if successfully registered.
 */
bool SimpleRegistration(SafeClient & _Client, int _SimIDA, int _SimIDB, std::vector<int> & _RegistrationMap, unsigned int _TryAngles = 12);

} // BG
