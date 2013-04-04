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
#include "camera.h"
#include "config.h"

extern int width, height;

Camera::Camera() {
	this->position = Vector3(0,0,0);
	this->target = Vector3(0,0,1);
	fov = M_PI/4;
}

Camera::Camera(const Vector3 &position, const Vector3 &target) {
	this->position = position;
	this->target = target;
	fov = M_PI/4;
}

void Camera::set_position(const Vector3 &position) {
	this->position = position;
}

void Camera::set_target(const Vector3 &target) {
	this->target = target;
}

void Camera::set_fov(double fov) {
	this->fov = fov;
}

Ray Camera::get_primary_ray(double x, double y) {
	Ray prim_ray;
	Vector3 dir;

	dir.x = x;
	dir.y = y; 
	dir.z = 1.0 / tan(fov / 2.0);

	Vector3 up(0,1,0); 
	Vector3 d = normalize(target-position);

	Vector3 i,j,k;
	
	i = cross(up, d);
	j = cross(d, i);
	k = d;

	Matrix4x4 m;

	m.matrix[0][0] = i.x; 
	m.matrix[1][0] = i.y;
	m.matrix[2][0] = i.z;

	m.matrix[0][1] = j.x;
	m.matrix[1][1] = j.y;
	m.matrix[2][1] = j.z;

	m.matrix[0][2] = k.x;
	m.matrix[1][2] = k.y;
	m.matrix[2][2] = k.z;

	prim_ray.origin = position;
	dir.transform(m);
	prim_ray.dir = dir * RAY_MAG;
	return prim_ray;
}

