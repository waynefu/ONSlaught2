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

#include "stdafx.h"
#ifndef PRECOMPILED_HEADERS_ARE_AVAILABLE
#endif
#include "Math.h"

using ONSlaught::Math::real_t;
using ONSlaught::Math::Matrix2;
using ONSlaught::Math::AffineMatrix2;
using ONSlaught::Math::Vector2;

Matrix2 ONSlaught::Math::rotation_matrix(real_t alpha){
	real_t m[] = {
		 cos(alpha), sin(alpha),
		-sin(alpha), cos(alpha)
	};
	return Matrix2(m);
}

Matrix2 ONSlaught::Math::scale_matrix(real_t x, real_t y){
	real_t m[] = {
		x, 0,
		0,  y
	};
	return Matrix2(m);
}

Matrix2 ONSlaught::Math::shear_matrix(real_t x, real_t y){
	real_t m[] = {
		1, x,
		y,  1
	};
	return Matrix2(m);
}

AffineMatrix2 ONSlaught::Math::to_affine(const Matrix2 &m, real_t offset_x, real_t offset_y){
	real_t temp[] = {
		m(0, 0), m(0, 1), offset_x,
		m(1, 0), m(1, 1), offset_y,
		0,       0,       0
	};
	return AffineMatrix2(temp);
}

Vector2 ONSlaught::Math::transform(const AffineMatrix2 &matrix, const Vector2 &v){
	real_t m[] = {
		v[0],
		v[1],
		1
	};
	EIGEN_VECTOR_TYPE(3) temp(m);
	temp = matrix * temp;
	m[0] = temp[0];
	m[1] = temp[1];
	return Vector2(m);
}

AffineMatrix2 ONSlaught::Math::affine_matrix_from_points(const Vector2 source_vectors[3], const Vector2 destination_vectors[3]){
	real_t m_a[] = {
		source_vectors[0].x(), source_vectors[1].x(), source_vectors[2].x(),
		source_vectors[0].y(), source_vectors[1].y(), source_vectors[2].y(),
		1,                     1,                     1
	};
	EIGEN_MATRIX_TYPE(3) a(m_a);
	a = a.inverse();
	real_t m_b[] = {
		destination_vectors[0].x(), destination_vectors[1].x(), destination_vectors[2].x(),
		destination_vectors[0].y(), destination_vectors[1].y(), destination_vectors[2].y()
	};
	Eigen::Matrix<real_t, 2, 3> b(m_b);
	b = b * a;
	AffineMatrix2 ret;
	for (int i = 0; i < 9; i++){
		int x = i%3;
		int y = i/3;
		auto &dst = ret(y, x);
		if (i < 6)
			dst = b(y, x);
		else
			dst = 0;
	}
	return ret;
}


int ONSlaught::Math::find_quadrant(real_t alpha, real_t &remainder){
	real_t truncated = floor(alpha / (pi / 2));
	int integer = (int)truncated;
	remainder = alpha - truncated;
	return integer >= 0 ? integer % 4 : (4 - (-integer) % 4) % 4;
}

AffineMatrix2 ONSlaught::Math::generate_matrix_for_quad_rotation(real_t alpha, real_t w, real_t h){
	Vector2 vectors[] = {
		Vector2(0, 0),
		Vector2(w, 0),
		Vector2(0, h),
		Vector2(w, h)
	};
	Matrix2 rot = rotation_matrix(alpha);
	for (auto &v : vectors)
		v = rot * v;
	real_t offset_x = vectors[0].x(),
		offset_y = vectors[0].y();
	for (int i = 1; i < 4; i++){
		offset_x = std::min(offset_x, vectors[i].x());
		offset_y = std::min(offset_y, vectors[i].y());
	}
	return to_affine(rot, -offset_x, -offset_y);

	/*
	real_t remainder;
	int quadrant = find_quadrant(alpha, remainder);
	AffineMatrix2 ret;
	real_t vertical_axis_length = quadrant % 2 ? width : height;
	switch (quadrant){
		case 0:
			ret = AffineMatrix2::Identity();
			break;
		case 1:
			ret = to_affine(rotation_matrix(pi/2), height);
			break;
		case 2:
			ret = to_affine(rotation_matrix(pi), width, height);
			break;
		case 3:
			ret = to_affine(rotation_matrix(pi*1.5), 0, width);
			break;
	}
	ret = to_affine(Matrix2::Identity(), -(rotation_matrix(remainder) * Vector2(0, vertical_axis_length)).x, 0) * ret;
	return ret;
	*/
}
