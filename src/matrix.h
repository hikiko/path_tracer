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

#ifndef MATRIX_H_
#define MATRIX_H_

class Vector3;

class Matrix4x4 {
public:
	double matrix[4][4];
	Matrix4x4();
	void set_translation(const Vector3 &tr);
	void set_rotation(const Vector3 &axis, double angle); 
	void set_scaling(const Vector3 &sc); 
	void transpose();
	void print();
};

#endif

