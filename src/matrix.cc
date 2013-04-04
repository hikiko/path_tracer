/*
Path_tracer - A CPU path tracer

Copyright (C) 2013 Eleni Maria Stea

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Author: Eleni Maria Stea <elene.mst@gmail.com>
*/

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "matrix.h"
#include "vector.h"

Matrix4x4::Matrix4x4() {
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			matrix[i][j] = (i == j ? 1 : 0);
		}
	}
}

void Matrix4x4::set_translation(const Vector3 &tr) {
	matrix[0][3] = tr.x;
	matrix[1][3] = tr.y;
	matrix[2][3] = tr.z;
}

void Matrix4x4::set_rotation(const Vector3 &axis, double angle) {
	double sina = sin(angle);
	double cosa = cos(angle);
	double invcosa = 1 - cosa;
	double sqx = axis.x * axis.x;
	double sqy = axis.y * axis.y;
	double sqz = axis.z * axis.z;
	
	matrix[0][0] = sqx + (1 - sqx) * cosa;
	matrix[0][1] = axis.x * axis.y * invcosa + axis.z * sina;
	matrix[0][2] = axis.x * axis.z * invcosa + axis.y * sina;
	matrix[1][0] = axis.x * axis.y * invcosa + axis.z * sina;
	matrix[1][1] = sqy + (1 - sqy) * cosa;
	matrix[1][2] = axis.y * axis.z * invcosa - axis.x * sina;
	matrix[2][0] = axis.x * axis.z * invcosa - axis.y * sina;
	matrix[2][1] = axis.y * axis.z * invcosa + axis.x * sina;
	matrix[2][2] = sqz + (1 - sqz) * cosa;
}

void Matrix4x4::set_scaling(const Vector3 &sc) {
	matrix[0][0] = sc.x;
	matrix[1][1] = sc.y;
	matrix[2][2] = sc.z;
}

void Matrix4x4::transpose() {
	double m[4][4];

	memcpy(m, matrix, sizeof m);

	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			matrix[i][j] = m[j][i];
		}
	}
}

void Matrix4x4::print() {
	printf("\n");
	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			printf("%f", matrix[i][j]);
			char nxt = (j%4 == 3 ? '\n' : '\t');
			printf("%c", nxt);
		}
	}
	printf("\n");
}

