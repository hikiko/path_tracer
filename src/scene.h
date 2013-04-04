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

#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include "light.h"
#include "camera.h"
#include "bbox.h"
#include "intinfo.h"
#include "object.h"

class Scene {
private: 
	std::vector<Object*> objects;
	Camera *cam;
	Color ambient;
	BBoxNode* bbroot;

public:
	std::vector<Object*> lights;

	Scene();
	~Scene();

	bool load(const char *fname);
	bool load(FILE *fp);
	
	void add_object(Object* object);
	void set_camera(Camera* cam);
	void set_ambient(const Color &amb);
	Color get_ambient();
	Camera* get_camera();
	bool intersection(const Ray &ray, IntInfo* inter);
	void build_bbtree();
};

#endif
