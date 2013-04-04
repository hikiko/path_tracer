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

#ifndef VECTOR_H_
#define VECTOR_H_

class Matrix4x4;

class Vector3 {
public:
	double x,y,z;
	Vector3();
	Vector3(double x, double y, double z);
	void transform(const Matrix4x4 &tm);
	void printv();
};

bool operator < (const Vector3 &a, const Vector3 &b);
bool operator > (const Vector3 &a, const Vector3 &b);

Vector3 operator + (const Vector3 &a, const Vector3 &b);
Vector3 operator - (const Vector3 &a, const Vector3 &b);
Vector3 operator - (const Vector3 &a);
Vector3 operator * (const Vector3 &a, const Vector3 &b);
Vector3 operator * (const Vector3 &a, double b);
Vector3 operator * (double b, const Vector3 &a);
Vector3 operator / (const Vector3 &a, double b);

const Vector3 &operator += (Vector3 &a, const Vector3 &b);

double length (const Vector3 &a);
double dot (const Vector3 &a, const Vector3 &b);
Vector3 cross (const Vector3 &a, const Vector3 &b);
Vector3 normalize (const Vector3 &a);

Vector3 reflect(const Vector3 &v, const Vector3 &n);

#endif

