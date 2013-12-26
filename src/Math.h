/*
* Copyright (c) 2013-2014, Helios (helios.vmg@gmail.com)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, 
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of the author may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*     * Products derived from this software may not be called "ONSlaught" nor
*       may "ONSlaught" appear in their names without specific prior written
*       permission from the author. 
*
* THIS SOFTWARE IS PROVIDED BY HELIOS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL HELIOS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef _MSC_VER
#pragma once
#endif

#ifndef MATH_H
#define MATH_H

#include <cmath>
#include "Eigen\Dense"

namespace ONSlaught{
namespace Math{

typedef float real_t;

static const real_t pi = (real_t)3.1415926535897932384626433832795;

inline bool is_near_zero(real_t r){
	const real_t tolerance = (real_t)0.001;
	return r < tolerance && r > -tolerance;
}

#define EIGEN_MATRIX_TYPE(x) Eigen::Matrix<real_t, x, x>
#define EIGEN_VECTOR_TYPE(x) Eigen::Matrix<real_t, x, 1>

typedef EIGEN_MATRIX_TYPE(2) Matrix2;
typedef EIGEN_MATRIX_TYPE(3) AffineMatrix2;
typedef EIGEN_VECTOR_TYPE(2) Vector2;
typedef Eigen::Matrix<unsigned, 2, 1> IntegerSizeVector2;
typedef Eigen::Matrix<int, 2, 1> IntegerPositionVector2;

Matrix2 rotation_matrix(real_t alpha);
Matrix2 scale_matrix(real_t x, real_t y);
Matrix2 shear_matrix(real_t x, real_t y);
AffineMatrix2 to_affine(const Matrix2 &, real_t offset_x = 0, real_t offset_y = 0);
Vector2 transform(const AffineMatrix2 &, const Vector2 &);

//Returns a matrix corresponding to the affine transformation f such that f(source_vectors[i]) = destination_vectors[i].
AffineMatrix2 affine_matrix_from_points(const Vector2 source_vectors[3], const Vector2 destination_vectors[3]);

//Returns the quadrant corresponding to alpha. remainder contains the radian remainder.
int find_quadrant(real_t alpha, real_t &remainder);
AffineMatrix2 generate_matrix_for_quad_rotation(real_t alpha, real_t width, real_t height);

}
}

#endif
