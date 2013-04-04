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

#ifndef CAMERA_H_
#define CAMERA_H_

#include "matrix.h"
#include "ray.h"
#include "vector.h"

class Camera{
private:
	Vector3 position;
	Vector3 target;
	double fov;
public:
	Camera();
	Camera(const Vector3 &position, const Vector3 &target);
	void set_position(const Vector3 &position);
	void set_target(const Vector3 &target);
	void set_fov(double fov);
	Ray get_primary_ray(double x, double y);
};

#endif
