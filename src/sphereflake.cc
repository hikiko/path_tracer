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

#include <float.h>
#include <string.h>
#include "sphereflake.h"
#include "config.h"

SphereFlake::SphereFlake(const Vector3 &center, double radius) {
	this->center = center;
	this->radius = radius;
	sph = 0;
	memset(subflakes, 0, sizeof subflakes);
}

SphereFlake::~SphereFlake() {
	for (size_t i = 0; i < 6; i++) {
		delete subflakes[i];
	}
	delete sph;
}

bool SphereFlake::intersection(const Ray &ray, IntInfo* i_info) const {
	if(ignore) {
		return false;
	}

#ifdef USE_BBOX
	if (!bbox.intersection(ray)) {
		return false;
	}
#endif

	IntInfo minsect;
	minsect.t = FLT_MAX;
	minsect.object = 0;

	//first check the sphere
	IntInfo tmp;
	if (sph->intersection(ray, &tmp) && tmp.t < minsect.t) {
		minsect = tmp;
	}

	for (size_t i = 0; i < 6; i++) {
		IntInfo tmp;
		if (subflakes[i] && subflakes[i]->intersection(ray, &tmp)) {
			if (tmp.t < minsect.t) {
				minsect = tmp;
			}
		}
	}

	if (minsect.object) {
		if (i_info) {
			*i_info = minsect;
			i_info->object = this;
		}
		return true;
	}
	return false;
}

void SphereFlake::calc_bbox() {
	double max_rad = 3.0 * radius;

	bbox.max = center + Vector3(max_rad, max_rad, max_rad);
	bbox.min = center - Vector3(max_rad, max_rad, max_rad);
}

Vector3 SphereFlake::sample() const {
	return Vector3(0, 0, 0);
}

static const Vector3 offs[] = {
	Vector3(1, 0, 0), Vector3(-1, 0, 0),
	Vector3(0, 1, 0), Vector3(0, -1, 0),
	Vector3(0, 0, 1), Vector3(0, 0, -1)
};

SphereFlake *create_sflake(const Vector3 &center, double radius, int iter) {
	if (!iter) return 0;

	SphereFlake *sflake = new SphereFlake(center, radius);
	sflake->calc_bbox();

	sflake->sph = new Sphere(center, radius);
	sflake->sph->calc_bbox();

	for (int i = 0; i < 6; i++) {
		double d = radius + radius / 2.0;
		Vector3 sub_pos = center + offs[i] * d;

		sflake->subflakes[i] = create_sflake(sub_pos, radius / 2.0, iter - 1);
	}
	return sflake;
}

