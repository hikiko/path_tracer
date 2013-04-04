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

#include "light.h"

PointLight::PointLight() {
	position = Vector3(0, 0, 0);
}

PointLight::PointLight(const Vector3 &pos, const Color &color) {
	this->position = pos;
}

bool PointLight::intersection(const Ray &ray, IntInfo* i_info) const {
	return false;
}

void PointLight::calc_bbox() {
#ifdef USE_BBOX
	if(!bbox.intersection(ray)) {
		return false;
	}
#endif
	bbox.max = bbox.min = position;
}

Vector3 PointLight::sample() const {
	return position;
}

bool PointLight::is_light() const {
	return true;
}
