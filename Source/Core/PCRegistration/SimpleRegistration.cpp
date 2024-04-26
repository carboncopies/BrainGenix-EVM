//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>
#include <limits>
#include <math.h>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <PCRegistration/SimpleRegistration.h>

namespace BG {

/**
 * Proximity difference between points in set A and set B.
 * A smaller value is a better match.
 */
float ProximityError(const std::vector<Vec3D>& _PointsA, const std::vector<Vec3D>& _PointsB, std::vector<size_t>* _FromANearestInB) {
	bool collect_indices = false;
	if (_FromANearestInB) {
		collect_indices = (_FromANearestInB->size() >= _PointsA.size());
	}
	float tot_distance = 0.0;
	for (size_t i = 0; i < _PointsA.size(); i++) {
		float d;
		size_t idx_in_B = NearestVec3D(_PointsA.at(i), _PointsB, &d);
		if (collect_indices) {
			_FromANearestInB->at(i) = idx_in_B;
		}
		tot_distance += d;
	}
	return tot_distance;
}

Vec3D BruteForceBestMatchAngles(const std::vector<Vec3D> _PointsA, const std::vector<Vec3D> _PointsB, std::vector<size_t>* _FromANearestInB, unsigned int try_angles) {
	float smallest_error = std::numeric_limits<float>::max();
	float fraction = 2.0*M_PI/float(try_angles);
	Vec3D best_angles;
	std::vector<size_t> fromAnearestinB;
	std::vector<size_t>* fromAnearestinBptr = nullptr;
	if (_FromANearestInB) {
		fromAnearestinB.resize(_PointsA.size());
		fromAnearestinBptr = &fromAnearestinB;
	}
	for (unsigned int x = 0; x < try_angles; x++) {
		for (unsigned int y = 0; y < try_angles; y++) {
			for (unsigned int z = 0; z < try_angles; z++) {
				float x_angle = float(x)*fraction;
				float y_angle = float(y)*fraction;
				float z_angle = float(z)*fraction;
				std::vector<Vec3D> rotatedB = RotatedSet3D(_PointsB, x_angle, y_angle, z_angle);
				float error = ProximityError(_PointsA, rotatedB, fromAnearestinBptr);
				if (error < smallest_error) {
					smallest_error = error;
					best_angles = Vec3D(x_angle, y_angle, z_angle);
					if (_FromANearestInB) {
						(*_FromANearestInB) = fromAnearestinB;
					}
				}
			}
		}
	}
	return best_angles;
}

/**
 * Use this if there are more points in A than in B to ensure that only
 * one point in A is mapped to a point in B, determined by rotated nearest
 * match. Excess map entries are set to -1.
 */
void MakeRegistrationMapUnique(const std::vector<Vec3D>& SomaCentersA, const std::vector<Vec3D>& SomaCentersB, const Vec3D& BestRotationAngles, std::vector<int> & _RegistrationMap) {
	for (size_t i = 0; i < _RegistrationMap.size(); i++) {
		int B_idx = _RegistrationMap[i];

		if (B_idx >= 0) {
			// Find all A indices that point to this B index.
			std::vector<size_t> BtoA;
			BtoA.emplace_back(i);
			for (size_t j = i+1; j < _RegistrationMap.size(); j++) {
				if (_RegistrationMap[j] == B_idx) {
					BtoA.emplace_back(j);
				}
			}

			// Make unique.
			if (BtoA.size() > 1) {
				// Find the pairing that is nearest.
				Vec3D PointInB = SomaCentersB[B_idx];
				Vec3D RotatedPointInB = PointInB.rotate_around_xyz(BestRotationAngles.x, BestRotationAngles.y, BestRotationAngles.z);
				std::vector<Vec3D> SubsetA;
				SubsetA.resize(BtoA.size());
				for (size_t i = 0; i < BtoA.size(); i++) {
					SubsetA[i] = SomaCentersA[BtoA[i]];
				}
				size_t BtoA_idx = NearestVec3D(RotatedPointInB, SubsetA);

				// Set the others to -1.
				for (size_t i = 0; i < BtoA.size(); i++) {
					if (i != BtoA_idx) {
						_RegistrationMap[BtoA[i]] = -1;
					}
				}
			}
		}
	}
}

/**
 * Uses centering and rotations to find the best registration of one network
 * onto the other and returns a cell ID map in accordance with the registration
 * that can be used for subsequent validation processes.
 * 
 * Note that the two simulations must be loaded before calling this function
 * with their numerical IDs.
 * 
 * @param _Client Reference to NES-connected client object.
 * @param _Config Configuration settings used.
 * @param _CollectedData A shared object in which KGT and EMU data is progressively collected.
 * @return True if successfully registered.
 */
bool SimpleRegistration(SafeClient & _Client, const ValidationConfig & _Config, DataCollector& _CollectedData) {

	_Client.Logger_->Log("Simple Registration of one simulation network onto another", 1);

	if (!_CollectedData.EnsureCentered(_Client, _Config)) {
		return false;
	}

	// 4. Check proximity score for various angle combinations to find best score.
	std::vector<size_t> FromANearestInB;
	_CollectedData.BestRotationAngles = BruteForceBestMatchAngles(_CollectedData.KGTData.SomaCenters, _CollectedData.EMUData.SomaCenters, &FromANearestInB, _Config.TryAngles);
	_CollectedData.KGT2Emu.resize(FromANearestInB.size());
	for (size_t i = 0; i < FromANearestInB.size(); i++) {
		_CollectedData.KGT2Emu[i] = FromANearestInB[i]; // from size_t to int
	}
	if (_CollectedData.KGTData.SomaCenters.size() > _CollectedData.EMUData.SomaCenters.size()) {
		MakeRegistrationMapUnique(_CollectedData.KGTData.SomaCenters, _CollectedData.EMUData.SomaCenters, _CollectedData.BestRotationAngles, _CollectedData.KGT2Emu);
	}
	_CollectedData.Registered = true;

	// 5. Return registered correspondence ID map.
	return true;
}

}; // Close Namespace BG
