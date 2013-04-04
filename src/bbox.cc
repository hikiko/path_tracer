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
#include "bbox.h"
#include "object.h"

//axis aligned bounding box
BBox::BBox(){}

BBox::BBox(const Vector3 &min, const Vector3 &max) {
	this->min = min;
	this->max = max;
}

/* ray-aabb intersection test based on:
 * "An Efficient and Robust Ray-Box Intersection Algorithm",
 * Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 * Journal of graphics tools, 10(1):49-54, 2005
 */
bool BBox::intersection(const Ray &ray) const {
	if(ray.origin > min && ray.origin < max) {
		return true;
	}

	Vector3 bbox[2] = {min, max};
	static const double t0 = 0.0;
	static const double t1 = 1.0;

	int xsign = (int)(ray.dir.x < 0.0);
	double invdirx = 1.0 / ray.dir.x;
	double tmin = (bbox[xsign].x - ray.origin.x) * invdirx;
	double tmax = (bbox[1 - xsign].x - ray.origin.x) * invdirx;

	int ysign = (int)(ray.dir.y < 0.0);
	double invdiry = 1.0 / ray.dir.y;
	double tymin = (bbox[ysign].y - ray.origin.y) * invdiry;
	double tymax = (bbox[1 - ysign].y - ray.origin.y) * invdiry;

	if((tmin > tymax) || (tymin > tmax)) {
		return false;
	}

	if(tymin > tmin) tmin = tymin;
	if(tymax < tmax) tmax = tymax;

	int zsign = (int)(ray.dir.z < 0.0);
	double invdirz = 1.0 / ray.dir.z;
	double tzmin = (bbox[zsign].z - ray.origin.z) * invdirz;
	double tzmax = (bbox[1 - zsign].z - ray.origin.z) * invdirz;

	if((tmin > tzmax) || (tzmin > tmax)) {
		return false;
	}

	if(tzmin > tmin) tmin = tzmin;
	if(tzmax < tmax) tmax = tzmax;

	return (tmin < t1) && (tmax > t0);
}

BBoxNode::BBoxNode(const BBox &bbox) {
	this->bbox = bbox;
}

BBoxNode::~BBoxNode(){
	for (int i = 0; i < (int)children.size(); i++) {
		delete children[i];
	}
}

bool BBoxNode::intersection(const Ray &ray, IntInfo* inf) const {

	//if ray doesn't hit bounding box
	if (!bbox.intersection(ray)) {
		return false;
	}

	IntInfo minsect;
	minsect.t = FLT_MAX;
	minsect.object = 0;
	
	//first find the closest intersection with the children (if any)
	if (!children.empty()) {
		for (int i = 0; i < (int)children.size(); i++) {
			IntInfo tmp;
			if (children[i]->intersection(ray, &tmp)) {
				if (tmp.t < minsect.t) {
					minsect = tmp;
				}
			}
		}
	}

	//and then check the objects in this box
	for (int i=0; i < (int)objects.size(); i++) {
		IntInfo tmp;
		if (objects[i]->intersection(ray, &tmp)) {
			if (tmp.t < minsect.t) {
				minsect = tmp;
			}
		}
	}

	if (minsect.object) {
		if (inf)
			*inf = minsect;
		return true;
	}
	return false;
}

void BBoxNode::add_child(BBoxNode* node) {
	children.push_back(node);
}

void BBoxNode::add_object(Object* obj) {
	objects.push_back(obj);
}

