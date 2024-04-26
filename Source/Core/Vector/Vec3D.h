//===========================================================//
// This file is part of the BrainGenix-EVM Validation System //
//===========================================================//

/*
    Description: This file provides utilities for creating and interacting with
    D vectors. This was copied from BrainGenix-NES VecTools.h.
*/

#pragma once

#include <cassert>
#include <cmath>
#include <string>
#include <vector>

namespace BG {

/**
 * @brief This struct defines a 3D object that will be used to represent
 * Cartesian coordinates.
 *
 */
struct Vec3D {

    float x{0}; //! X-coordinate.
    float y{0}; //! Y-coordinate.
    float z{0}; //! Z-coordinate.

    //! Constructors
    Vec3D();
    Vec3D(float _x, float _y, float _z);
    Vec3D(const std::vector<float> & vec);

    //! Operators
    //! Vector + Vector
    Vec3D operator+(const Vec3D &other) const;
    Vec3D operator+=(const Vec3D &other);
    //! Vector - Vector
    Vec3D operator-(const Vec3D &other) const;
    Vec3D operator-=(const Vec3D &other);
    //! Vector * Vector (Elementwise)
    Vec3D operator*(const Vec3D other) const;
    //! scalar * Vector
    Vec3D operator*(const float other) const;
    Vec3D operator*=(const float other);
    //! Vector / scalar
    Vec3D operator/(const float other) const;
    Vec3D operator/=(const float other);
    //! Vector == Vector
    bool operator==(const Vec3D &other) const;
    //! Vector != Vector
    bool operator!=(const Vec3D &other) const;

    //! Indexed access to x, y, z (modulo 3)
    float& operator[](size_t idx);

    //! Vector <= Vector (all elements)
    bool AllElementsLTE(const Vec3D &other) const;
    
    //! Vector >= Vector (all elements)
    bool AllElementsGTE(const Vec3D &other) const;


    //! Distance between two vectors
    float Distance(const Vec3D &other) const;

    //! Dot product of two vectors
    float Dot(const Vec3D &other) const;

    //! Cross product of two vectors
    Vec3D Cross(const Vec3D &other) const;

    //! Maximum coordinate value
    float Max() const;

    //! Minimum coordinate value
    float Min() const;

    //! Return string representation of vector.
    std::string str(int precision=4) const;

    //! Convert cartesian 3D vector to spherical coordinates.
    //! Returns (r, theta, phi) where theta is rotation around x-axis
    //! and phi is rotation around z-axis.
    Vec3D cartesianToSpherical() const;

    float r() const;
    float theta() const;
    float phi() const;

    //! Convert spherical 3D vector to cartesian coordinates.
    //! Returns (x, y, z) from (r, theta, phi).
    Vec3D sphericalToCartesian() const;

    Vec3D rotate_around_x(float _xangle) const;
    Vec3D rotate_around_y(float _yangle) const;
    Vec3D rotate_around_z(float _zangle) const;

    Vec3D rotate_around_xyz(float _xangle, float _yangle, float _zangle) const;

    //! Uses three concatenated rotation matrices to rotate a 3D point around the
    //! x-axiz, y_axis and z-axis in cartesian coordinates.
    Vec3D rotate_around_zyx(float _xangle, float _yangle, float _zangle) const;
};

Vec3D GeometricCenter(const std::vector<Vec3D> _Vectors);

size_t NearestVec3D(const Vec3D & _Point, const std::vector<Vec3D> & _Candidates, float* _NearestDistance = nullptr);

std::vector<Vec3D> RotatedSet3D(const std::vector<Vec3D>& _Points, float _xangle, float _yangle, float _zangle);

}; // namespace BG
