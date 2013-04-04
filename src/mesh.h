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

#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include "object.h"
#include "vector.h"
#include "bbox.h"

enum MeshPrim {
	MESH_PRIM_TRI = 3,
	MESH_PRIM_QUAD = 4
};

struct Vertex {
	Vector3 pos;
	Vector3 norm;
	// Vector3 tex;
};

class Face {
public:
	Vertex v[4];
	Vector3 norm;	// face normal

	void calc_normal();
	Vector3 sample(MeshPrim prim) const;
};

class Mesh : public Object {
protected:
	MeshPrim prim;
	std::vector<Face> faces;

	bool (*face_intersection)(const Face*, const Ray&, IntInfo*);

public:

	Mesh(MeshPrim prim = MESH_PRIM_TRI);
	virtual ~Mesh();

	void set_primitive(MeshPrim prim);
	MeshPrim get_primitive() const;

	void add_face(const Face &face);
	int get_face_count() const;
	Face *get_face(int idx);
	const Face *get_face(int idx) const;

	virtual bool intersection(const Ray &ray, IntInfo *i_info) const;
	virtual void calc_bbox();
	virtual Vector3 sample() const;
};

#endif
