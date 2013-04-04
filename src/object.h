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

#ifndef OBJECT_H_
#define OBJECT_H_

#include "color.h"
#include "intinfo.h"
#include "ray.h"
#include "bbox.h"

struct Material {
	Color kd;
	Color ks;
	Color ke;
	double specexp;
	double kr;
};

class Object {
protected:
	Material material;
	BBox bbox;

public:
	bool ignore;

	Object();

	virtual bool intersection(const Ray &ray, IntInfo* i_info) const = 0;

	Material* get_material();
	const Material* get_material() const;
	virtual bool is_light() const;

	virtual void calc_bbox() = 0;
	virtual Vector3 sample() const = 0;
};

#endif
