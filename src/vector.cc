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
#include "vector.h"
#include "matrix.h"

Vector3::Vector3() {
	x = 0;
	y = 0;
	z = 0;
}

Vector3::Vector3(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

void Vector3::transform(const Matrix4x4 &tm) {
	double x1 = tm.matrix[0][0]*x + tm.matrix[0][1]*y + tm.matrix[0][2]*z + tm.matrix[0][3];
	double y1 = tm.matrix[1][0]*x + tm.matrix[1][1]*y + tm.matrix[1][2]*z + tm.matrix[1][3];
	double z1 = tm.matrix[2][0]*x + tm.matrix[2][1]*y + tm.matrix[2][2]*z + tm.matrix[2][3];
	x = x1;
	y = y1;
	z = z1;
}

void Vector3::printv() {
	printf("%f\t%f\t%f\n", x, y, z);
}


bool operator < (const Vector3 &a, const Vector3 &b) {
	return a.x < b.x && a.y < b.y && a.z < b.z;
}

bool operator > (const Vector3 &a, const Vector3 &b) {
	return a.x > b.x && a.y > b.y && a.z > b.z;
}

Vector3 operator + (const Vector3 &a, const Vector3 &b) {
	return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector3 operator - (const Vector3 &a, const Vector3 &b) {
	return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector3 operator - (const Vector3 &a) {
	return Vector3(-a.x, -a.y, -a.z);
}

Vector3 operator * (const Vector3 &a, const Vector3 &b) {
	return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vector3 operator * (const Vector3 &a, double b) {
	return Vector3(a.x*b, a.y*b, a.z*b);
}

Vector3 operator * (double b, const Vector3 &a) {
	return Vector3(a.x*b, a.y*b, a.z*b);
}

Vector3 operator / (const Vector3 &a, double b) {
	return Vector3(a.x / b, a.y / b, a.z / b);
}

const Vector3 &operator += (Vector3 &a, const Vector3 &b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

double length(const Vector3 &a) {
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

double dot(const Vector3 &a, const Vector3 &b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector3 cross(const Vector3 &a, const Vector3 &b) {
	return Vector3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

Vector3 normalize(const Vector3 &vec) {
	double mag = sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
	return vec / mag;
}

Vector3 reflect(const Vector3 &v, const Vector3 &n) {
	return 2.0 * dot(v, n) * n - v;
}
