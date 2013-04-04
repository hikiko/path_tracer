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
#include <stdlib.h>
#include "brdf.h"
#include "config.h"
#include "matrix.h"


double phong(const Vector3 &indir, const Vector3 &outdir, const Vector3 &n, double specexp) {
	Vector3 refindir = reflect(indir, n);
	double s = dot(refindir, outdir);
	if (s < 0.0) {
		return 0.0;
	}
	return pow(s, specexp);
}

double lambert(const Vector3 &indir, const Vector3 &n) {
	double d = dot(n, indir);
	if (d < 0.0) {
		d = 0.0;
	}
	return d;
}

Vector3 sample_lambert(const Vector3 &n) {
	double rndx, rndy, rndz;
	double magnitude;
	do {
		rndx = 2.0 * (double) rand() / RAND_MAX - 1.0;
		rndy = 2.0 * (double) rand() / RAND_MAX - 1.0;
		rndz = 2.0 * (double) rand() / RAND_MAX - 1.0;
		magnitude = sqrt(rndx * rndx + rndy * rndy + rndz * rndz);
	} while (magnitude > 1.0); 

	Vector3 rnd_dir(rndx / magnitude, rndy / magnitude, rndz / magnitude);
	
	if (dot(rnd_dir, n) < 0) {
		return -rnd_dir;
	}
	
	return rnd_dir;
}

Vector3 sample_phong(const Vector3 &outdir, const Vector3 &n, double specexp) {
	Matrix4x4 mat;
	Vector3 ldir = normalize(outdir);

	Vector3 ref = reflect(ldir, n);

	double ndotl = dot(ldir, n);

	if(1.0 - ndotl > EPSILON) {
		Vector3 ivec, kvec, jvec;

		// build orthonormal basis
		if(fabs(ndotl) < EPSILON) {
			kvec = -normalize(ldir);
			jvec = n;
			ivec = cross(jvec, kvec);
		} else {
			ivec = normalize(cross(ldir, ref));
			jvec = ref;
			kvec = cross(ref, ivec);
		}

		mat.matrix[0][0] = ivec.x;
		mat.matrix[1][0] = ivec.y;
		mat.matrix[2][0] = ivec.z;

		mat.matrix[0][1] = jvec.x;
		mat.matrix[1][1] = jvec.y;
		mat.matrix[2][1] = jvec.z;

		mat.matrix[0][2] = kvec.x;
		mat.matrix[1][2] = kvec.y;
		mat.matrix[2][2] = kvec.z;
	}

	double rnd1 = (double)rand() / RAND_MAX;
	double rnd2 = (double)rand() / RAND_MAX;

	double phi = acos(pow(rnd1, 1.0 / (specexp + 1)));
	double theta = 2.0 * M_PI * rnd2;

	Vector3 v;
	v.x = cos(theta) * sin(phi);
	v.y = cos(phi);
	v.z = sin(theta) * sin(phi);
	v.transform(mat);

	return v;
}
