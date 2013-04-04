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

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "kdtree.h"
#include "mesh.h"

static bool tri_intersection(const Face *face, const Ray &ray, IntInfo *i_info);
static bool quad_intersection(const Face *face, const Ray &ray, IntInfo *i_info);


void Face::calc_normal()
{
	Vector3 a, b;

	a = v[1].pos - v[0].pos;
	b = v[2].pos - v[0].pos;

	norm = normalize(cross(b, a));
}

Vector3 Face::sample(MeshPrim prim) const {
	Vector3 smpl;

	if (prim == MESH_PRIM_TRI) {
		double a, b, c;
		do {
			a = (double) rand() / RAND_MAX;
			b = (double) rand() / RAND_MAX;
			c = (double) rand() / RAND_MAX;
		} while(a + b + c > 1.0);

		smpl = v[0].pos * a + v[1].pos * b + v[2].pos * c;
	}
	else {
		Vector3 edge_a = v[1].pos - v[0].pos;
		Vector3 edge_b = v[2].pos - v[0].pos;
		
		double a = (double) rand() / RAND_MAX;
		double b = (double) rand() / RAND_MAX;

		smpl = (a * edge_a) + (b * edge_b) + v[0].pos;
	}
	return smpl;
}

Mesh::Mesh(MeshPrim prim) {
	set_primitive(prim);
}

Mesh::~Mesh() {
}

void Mesh::set_primitive(MeshPrim prim) {
	this->prim = prim;

	switch(prim) {
	case MESH_PRIM_TRI:
		face_intersection = tri_intersection;
		break;

	case MESH_PRIM_QUAD:
		face_intersection = quad_intersection;
		break;

	default:
		// this shouldn't happen
		fprintf(stderr, "invalid primitive\n");
		abort();
	}
}

void Mesh::add_face(const Face &face) {
	faces.push_back(face);
}

int Mesh::get_face_count() const {
	return (int)faces.size();
}

Face *Mesh::get_face(int idx) {
	if(idx < 0 || idx >= (int)faces.size()) {
		return 0;
	}
	return &faces[idx];
}

const Face *Mesh::get_face(int idx) const {
	if(idx < 0 || idx >= (int)faces.size()) {
		return 0;
	}
	return &faces[idx];
}

bool Mesh::intersection(const Ray &ray, IntInfo *i_info) const {
	if(ignore) {
		return false;
	}

	// first check if the ray intersects the bounding box of the mesh
#ifdef USE_BBOX
	if(!bbox.intersection(ray)) {
		return false;
	}
#endif

	bool found = false;

	IntInfo nearest;
	nearest.t = DBL_MAX;

	// TODO implement space subdivision
	for(size_t i=0; i<faces.size(); i++) {
		IntInfo inf;
		if(face_intersection(&faces[i], ray, &inf) && inf.t < nearest.t) {
			nearest = inf;
			found = true;
		}
	}

	if(!found) {
		return false;
	}

	if(i_info) {
		*i_info = nearest;
		i_info->object = this;
	}
	return true;
}

void Mesh::calc_bbox() {
	bbox.max = Vector3(-DBL_MAX, -DBL_MAX, -DBL_MAX);
	bbox.min = Vector3(DBL_MAX, DBL_MAX, DBL_MAX);

	for(size_t i=0; i<faces.size(); i++) {
		for(int j=0; j<prim; j++) {
			Vector3 v = faces[i].v[j].pos;

			if(v.x < bbox.min.x) bbox.min.x = v.x;
			if(v.y < bbox.min.y) bbox.min.y = v.y;
			if(v.z < bbox.min.z) bbox.min.z = v.z;

			if(v.x > bbox.max.x) bbox.max.x = v.x;
			if(v.y > bbox.max.y) bbox.max.y = v.y;
			if(v.z > bbox.max.z) bbox.max.z = v.z;
		}
	}
}

Vector3 Mesh::sample() const {
	int rnd = (int) ((double) rand() / ((double)RAND_MAX + 1) * (double)faces.size());
	assert(rnd < (int) faces.size());
	const Face* rnd_face = &faces[rnd];
	return rnd_face->sample(prim);
}

static Vector3 bary_coords(const Vector3 &pt, const Face *face)
{
	Vector3 bc;

	// find the area of the whole triangle
	Vector3 vi = face->v[1].pos - face->v[0].pos;
	Vector3 vj = face->v[2].pos - face->v[0].pos;
	double area = fabs(dot(cross(vi, vj), face->norm) / 2.0);
	if(area < 1e-8) {
		return bc;	// zero-area triangle (points coplanar or coincident)
	}

	// three vectors radiating from the point to the vertices;
	Vector3 pv0 = face->v[0].pos - pt;
	Vector3 pv1 = face->v[1].pos - pt;
	Vector3 pv2 = face->v[2].pos - pt;

	// calculate the areas of each sub-triangle
	double a0 = fabs(dot(cross(pv1, pv2), face->norm) / 2.0);
	double a1 = fabs(dot(cross(pv2, pv0), face->norm) / 2.0);
	double a2 = fabs(dot(cross(pv0, pv1), face->norm) / 2.0);

	bc.x = a0 / area;
	bc.y = a1 / area;
	bc.z = a2 / area;
	return bc;
}

static bool tri_intersection(const Face *face, const Ray &ray, IntInfo *i_info)
{
	/* Step 1: solve the plane equation to find intersection with the triangle
	   plane */

	/* triangle vertices */
	Vector3 pointA = face->v[0].pos;
	Vector3 pointB = face->v[1].pos;
	Vector3 pointC = face->v[2].pos;

	Vector3 normalA = face->v[0].norm;
	Vector3 normalB = face->v[1].norm;
	Vector3 normalC = face->v[2].norm;

	Vector3 n = face->norm;

	/* in case that the points A, B, C belong to the same line --A--B--C--
	 we don't have a triangle! */
	if (n.x == 0 && n.y == 0 && n.z == 0)
		return false;

	/*plane normal and parameter d*/
	double d = dot(n, pointA);

	/* find the ray-plane intersection - if any */
	double ndir = dot(n, ray.dir);
	if (ndir == 0)
		return false;
	double t = (d - dot(n, ray.origin))/ndir;
	if(t < EPSILON || t > 1.0) {
		return false;
	}

	Vector3 point = ray.origin + ray.dir * t; /* intersection point */

	// calculate barycentric coordinates of the intersection point
	Vector3 bc = bary_coords(point, face);

	double bc_sum = bc.x + bc.y + bc.z;
	if(bc_sum < -1e-8 || bc_sum > 1.0 + 1e-8) {
		/* if the sum of the barycentric coordinates of the intersection point
		 * are outside of the interval [0, 1], then the point is not inside
		 * the triangle.
		 */
		return false;
	}

	if (i_info) {
		/* calculate the normal at the intersection point */
		i_info->normal = normalize(bc.x * normalA + bc.y * normalB + bc.z * normalC);
		i_info->i_point = point;
		i_info->t = t;
	}

	return true;
}

static bool quad_intersection(const Face *face, const Ray &ray, IntInfo *i_info)
{
	Face tri = *face;

	if(tri_intersection(&tri, ray, i_info)) {
		return true;
	}

	tri.v[1] = face->v[2];
	tri.v[2] = face->v[3];
	return tri_intersection(&tri, ray, i_info);
}

static KDNode* construct_kdtree();
static void add_node();
