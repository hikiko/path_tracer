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

#include <stdlib.h>
#include "math.h"
#include "sphere.h"
#include "config.h"

Sphere::Sphere() {
	center = Vector3(0,0,0);
	radius = 1;
}

Sphere::Sphere(const Vector3 &center, double radius) {
	this->center = center;
	this->radius = radius;
}

bool Sphere::intersection(const Ray &ray, IntInfo* i_info) const {
	if(ignore) {
		return false;
	}

	// first check if the ray intersects the bounding box of the sphere
	// this is marginally faster (measured)
#ifdef USE_BBOX
	if(!bbox.intersection(ray)) {
		return false;
	}
#endif

	double a = dot(ray.dir, ray.dir);
	double b = 2 * ray.dir.x * (ray.origin.x - center.x) +
		2 * ray.dir.y * (ray.origin.y - center.y) +
		2 * ray.dir.z * (ray.origin.z - center.z);
	double c = dot(center, center) + dot(ray.origin, ray.origin) + 
		2 * dot(-center, ray.origin) - radius * radius;
	
	double discr = (b * b - 4 * a * c);

	if (discr < 0.0) {
		return false;
	}

	double sqrt_discr = sqrt(discr);
	double t1 = (-b + sqrt_discr) / (2.0 * a);
	double t2 = (-b - sqrt_discr) / (2.0 * a);

	if (t1 < EPSILON) t1 = t2;
	if (t2 < EPSILON) t2 = t1;

	double t = t1 < t2 ? t1 : t2;

	if (t < EPSILON || t > 1.0) {
		return false;
	}

	if (i_info) {
		i_info->t = t;
		i_info->i_point = ray.origin + ray.dir * t;
		i_info->normal = (i_info->i_point - center) / radius;
		i_info->object = this;
	}
	return true;
}

void Sphere::calc_bbox() {
	bbox.max = center + Vector3(radius, radius, radius);
	bbox.min = center - Vector3(radius, radius, radius);
}

Vector3 Sphere::sample() const {
	double rndx, rndy, rndz;
	double magnitude;
	do {
		rndx = 2.0 * (double) rand() / RAND_MAX - 1.0;
		rndy = 2.0 * (double) rand() / RAND_MAX - 1.0;
		rndz = 2.0 * (double) rand() / RAND_MAX - 1.0;
		magnitude = sqrt(rndx * rndx + rndy * rndy + rndz * rndz);
	} while (magnitude > 1.0); 

	Vector3 rnd_point(rndx / magnitude, rndy / magnitude, rndz / magnitude);
	return rnd_point * radius + center;
}
