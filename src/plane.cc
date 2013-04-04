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
#include "plane.h"
#include "config.h"

Plane::Plane() {
	normal = Vector3(0,1,0);
	distance = 0;
}

Plane::Plane(const Vector3 &normal, double distance) {
	this->normal = normalize(normal);
	this->distance = distance;
}

bool Plane::intersection(const Ray &ray, IntInfo* inf) const {
	if(ignore) {
		return false;
	}

	double n_dot_dir = dot(ray.dir, normal);

	if (fabs(n_dot_dir) < EPSILON) {
		return false;
	}

	Vector3 v = normal * distance;
	Vector3 vorigin = v - ray.origin;

	double n_dot_vo = dot(vorigin, normal);
	double t = n_dot_vo / n_dot_dir; 

	if (t < EPSILON || t > 1.0) {
		return false;
	}

	if (inf) {
		inf->t = t;
		inf->i_point = ray.origin + ray.dir * t;
		inf->normal = normal;
		inf->object = this;
	}
	return true;
}

void Plane::calc_bbox() {
	/* the plane is infinite, so let's just define the bounding box to
	 * enclose all usable space (i.e. as far as the rays reach approximately)
	 */
	bbox.max = Vector3(RAY_MAG, RAY_MAG, RAY_MAG);
	bbox.min = -bbox.max;
}

Vector3 Plane::sample() const{
	return Vector3(0, 0, 0);
}
