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

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include "config.h"
#include "scene.h"
#include "sphere.h"
#include "plane.h"
#include "sphereflake.h"
#include "mesh.h"
#include "camera.h"
#include "light.h"

#define DEG_TO_RAD(x)	(M_PI * (x) / 180.0)

static Sphere *load_sphere(const char *line);
static Plane *load_plane(const char *line);
static SphereFlake *load_sphflake(const char *line);
static Mesh *load_mesh(const char *line);
static bool load_mesh_data(Mesh *mesh, const char *fname, const Vector3 &pos, const Matrix4x4 &rot, const Vector3 &scale);
static char *strip_space(char *buf);
static Camera *load_camera(const char *line);
static PointLight *load_light(const char *line);

Scene::Scene(){
	cam = 0;
	ambient = Color(0, 0, 0);
	bbroot = 0;
}

Scene::~Scene() {
	for (int i = 0; i < (int) objects.size(); i++) {
		if(!objects[i]->is_light()) {
			delete objects[i];
		}
	}

	for (int i = 0; i < (int) lights.size(); i++) {
		delete lights[i];
	}

	if (bbroot) {
		delete bbroot;
	}
}

bool Scene::load(const char *fname) {
	FILE *fp;

	if(!(fp = fopen(fname, "r"))) {
		return false;
	}

	bool res = load(fp);
	fclose(fp);
	return res;
}

#define ERROR(l, n)	\
	fprintf(stderr, "error in line %d: \"%s\", ignoring.\n", n, l)

bool Scene::load(FILE *fp) {
	char line[1024];
	Sphere *sph;
	SphereFlake *sflake;
	Plane *plane;
	Camera *cam;
	PointLight *lt;
	Mesh *mesh;

	int lnum = 0;
	while(fgets(line, sizeof line, fp)) {
		lnum++;

		if(line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
			continue;
		}

		switch(line[0]) {
		case 's':
			if((sph = load_sphere(line))) {
				add_object(sph);
			} else {
				ERROR(line, lnum);
			}
			break;

		case 'p':
			if((plane = load_plane(line))) {
				add_object(plane);
			} else {
				ERROR(line, lnum);
			}
			break;

		case 'f':
			if((sflake = load_sphflake(line))) {
				add_object(sflake);
			} else {
				ERROR(line, lnum);
			}
			break;

		case 'l':
			if((lt = load_light(line))) {
				lights.push_back(lt);
			} else {
				ERROR(line, lnum);
			}
			break;

		case 'c':
			if((cam = load_camera(line))) {
				set_camera(cam);
			} else {
				ERROR(line, lnum);
			}
			break;

		case 'm':
			if((mesh = load_mesh(line))) {
				add_object(mesh);
			}
			else {
				ERROR(line, lnum);
			}
			break;

		default:
			ERROR(line, lnum);
		}
	}

	return true;
}


void Scene::add_object(Object* object) {
	objects.push_back(object);
	if (object->is_light()) {
		lights.push_back(object);
	}
}

bool Scene::intersection(const Ray &ray, IntInfo* inter) {
	if(!bbroot) {
		build_bbtree();
	}

	return bbroot->intersection(ray, inter);
}

void Scene::set_camera(Camera* camera) {
	cam = camera;
}

Camera* Scene::get_camera() {
	return cam;
}

void Scene::set_ambient(const Color &amb) {
	ambient = amb;
}

Color Scene::get_ambient() {
	return ambient;
}

void Scene::build_bbtree() {
	/* since we have infinite planes make the root bounding box *LARGE*
	 * (not quite correct but works for our purposes, the ray
	 * doesn't go to infinity anyway).
	 */
	Vector3 max(RAY_MAG, RAY_MAG, RAY_MAG);
	Vector3 min = -max;

	bbroot = new BBoxNode(BBox(min, max));

	for(size_t i = 0; i < objects.size(); i++) {
		objects[i]->calc_bbox();
		bbroot->add_object(objects[i]);
	}
}

static Sphere *load_sphere(const char *line) {
	float x, y, z, dr, dg, db, sr, sg, sb, rad, specexp, kr;
	float er, eg, eb;
	Sphere *sph;

	int res = sscanf(line, "s c(%f %f %f) r(%f) kd(%f %f %f) ks(%f %f %f) s(%f) kr(%f) ke(%f %f %f)\n",
			&x, &y, &z, &rad, &dr, &dg, &db, &sr, &sg, &sb, &specexp, &kr, &er, &eg, &eb);
	if(res < 15) {
		return 0;
	}

	sph = new Sphere(Vector3(x, y, z), rad);
	Material *mat = sph->get_material();

	mat->kd = Vector3(dr, dg, db);
	mat->ks = Vector3(sr, sg, sb);
	mat->ke = Vector3(er, eg, eb);
	mat->specexp = specexp;
	mat->kr = kr;
	return sph;
}

static Plane *load_plane(const char *line) {
	float nx, ny, nz, dr, dg, db, sr, sg, sb, dist, specexp, kr;
	float er, eg, eb;
	Plane *plane;

	int res = sscanf(line, "p n(%f %f %f) d(%f) kd(%f %f %f) ks(%f %f %f) s(%f) kr(%f) ke(%f %f %f)\n",
			&nx, &ny, &nz, &dist, &dr, &dg, &db, &sr, &sg, &sb, &specexp, &kr, &er, &eg, &eb);
	if(res < 15) {
		return 0;
	}

	plane = new Plane(Vector3(nx, ny, nz), dist);
	Material *mat = plane->get_material();

	mat->kd = Vector3(dr, dg, db);
	mat->ks = Vector3(sr, sg, sb);
	mat->ke = Vector3(er, eg, eb);
	mat->specexp = specexp;
	mat->kr = kr;
	return plane;
}

static SphereFlake *load_sphflake(const char *line) {
	float x, y, z, dr, dg, db, sr, sg, sb, rad, specexp, kr;
	float er, eg, eb;
	int iter;

	int res = sscanf(line, "f c(%f %f %f) r(%f) i(%d) kd(%f %f %f) ks(%f %f %f) s(%f) kr(%f) ke(%f %f %f)\n",
			&x, &y, &z, &rad, &iter, &dr, &dg, &db, &sr, &sg, &sb, &specexp, &kr, &er, &eg, &eb);
	if(res < 16) {
		return 0;
	}

	SphereFlake *sflake = create_sflake(Vector3(x, y, z), rad, iter);
	Material *mat = sflake->get_material();

	mat->kd = Vector3(dr, dg, db);
	mat->ks = Vector3(sr, sg, sb);
	mat->ke = Vector3(er, eg, eb);
	mat->specexp = specexp;
	mat->kr = kr;
	return sflake;
}

static Mesh *load_mesh(const char *line) {
	char fname[512];
	float x, y, z, rx, ry, rz, angle, sx, sy, sz;
	float dr, dg, db, sr, sg, sb, specexp, kr;
	float er, eg, eb;

	int res = sscanf(line, "m %s pos(%f %f %f) rot(%f %f %f %f) scale(%f %f %f) kd(%f %f %f) ks(%f %f %f) s(%f) kr(%f) ke(%f %f %f)\n",
			fname, &x, &y, &z, &angle, &rx, &ry, &rz, &sx, &sy, &sz,
			&dr, &dg, &db, &sr, &sg, &sb, &specexp, &kr, &er, &eg, &eb);
	if(res < 22) {
		return 0;
	}

	Vector3 pos(x, y, z), scale(sx, sy, sz);

	Matrix4x4 rot;
	rot.set_rotation(Vector3(rx, ry, rz), DEG_TO_RAD(angle));

	Mesh *mesh = new Mesh;
	if(!load_mesh_data(mesh, fname, pos, rot, scale)) {
		delete mesh;
		return 0;
	}

	Material *mat = mesh->get_material();
	mat->kd = Vector3(dr, dg, db);
	mat->ks = Vector3(sr, sg, sb);
	mat->ke = Vector3(er, eg, eb);
	mat->specexp = specexp;
	mat->kr = kr;
	return mesh;
}

static bool load_mesh_data(Mesh *mesh, const char *fname, const Vector3 &pos, const Matrix4x4 &rot, const Vector3 &scale) {
	FILE *fp;
	char buf[256];
	int prim;
	Face face;

	std::vector<Vector3> verts, normals;

	if(!(fp = fopen(fname, "r"))) {
		fprintf(stderr, "failed to open mesh data file: %s: %s\n", fname, strerror(errno));
		return false;
	}

	if(!fgets(buf, sizeof buf, fp)) {
		goto err;
	}
	if(sscanf(buf, "MESH%d\n", &prim) != 1) {
		goto err;
	}

	mesh->set_primitive((MeshPrim)prim);

	while(fgets(buf, sizeof buf, fp)) {
		char *line = strip_space(buf);
		float x, y, z;
		int vidx[4], nidx[4], res;
		Vector3 vec;

		if(!line[0] || line[0] == '#') {
			continue;
		}

		switch(line[0]) {
		case 'v':
			if(sscanf(line, "v %f %f %f", &x, &y, &z) < 3) {
				goto err;
			}
			vec = Vector3(x * scale.x, y * scale.y, z * scale.z);
			vec.transform(rot);
			verts.push_back(vec + pos);
			break;

		case 'n':
			if(sscanf(line, "n %f %f %f", &x, &y, &z) < 3) {
				goto err;
			}
			vec = Vector3(x, y, z);
			vec.transform(rot);
			normals.push_back(normalize(vec));
			break;

		case 'f':
			res = sscanf(line, "f %d/%d %d/%d %d/%d %d/%d", vidx, nidx, vidx + 1, nidx + 1,
					vidx + 2, nidx + 2, vidx + 3, nidx + 3);
			if(res != prim * 2) {
				goto err;
			}

			for(int i=0; i<prim; i++) {
				face.v[i].pos = verts[vidx[i] - 1];
				face.v[i].norm = normals[nidx[i] - 1];
			}
			face.calc_normal();
			mesh->add_face(face);
			break;

		default:
			goto err;
		}
	}

	fclose(fp);
	return true;

err:
	fprintf(stderr, "failed to read mesh data file: %s: invalid format\n", fname);
	fclose(fp);
	return false;
}

static char *strip_space(char *buf)
{
	while(*buf && isspace(*buf)) {
		buf++;
	}

	char *end = buf + strlen(buf) - 1;
	if(end < buf) {
		end = buf;
	}

	while(isspace(*end)) {
		end--;
	}

	if(*end) end[1] = 0;
	return buf;
}

static Camera *load_camera(const char *line) {
	float x, y, z, tx, ty, tz, fov;

	int res = sscanf(line, "c p(%f %f %f) t(%f %f %f) fov(%f)\n", &x, &y, &z, &tx, &ty, &tz, &fov);
	if(res < 7) {
		return 0;
	}

	Camera *cam = new Camera(Vector3(x, y, z), Vector3(tx, ty, tz));
	cam->set_fov(M_PI * fov / 180.0);

	return cam;
}

static PointLight *load_light(const char *line) {
	float x, y, z, r, g, b;

	int res = sscanf(line, "l p(%f %f %f) c(%f %f %f)\n", &x, &y, &z, &r, &g, &b);
	if(res < 6) {
		return 0;
	}

	PointLight *lt = new PointLight;
	lt->position = Vector3(x, y, z);
	lt->get_material()->ke = Vector3(r, g, b);

	return lt;
}
