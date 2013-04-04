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

#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "brdf.h"
#include "camera.h"
#include "color.h"
#include "config.h"
#include "intinfo.h"
#include "light.h"
#include "matrix.h"
#include "object.h"
#include "plane.h"
#include "ray.h"
#include "scene.h"
#include "sphere.h"
#include "sphereflake.h"
#include "vector.h"
#include "vector.h"

#define MAX(a, b)	((a) > (b) ? (a) : (b))
#define DEGTORAD(x)	(M_PI * x / 180.0)

int dbg_curx, dbg_cury;

int width = 512;
int height = 512;
uint32_t *image;
int rays_ppxl = 4;
int pix_subdiv;
double inv_gamma = 1.0;

SDL_Surface *fbsurf;
Scene scene;
bool use_sdl = true;

Color trace(const Ray &ray, int depth);
Color shade(const Ray &ray, IntInfo *min_info, int depth);
Color avg_color(double pxl_width, double pxl_height, double x, double y, int depth);
Vector3 reflect(const Vector3 &l, const Vector3 &n);

void update();
void cleanup();
void render();
void render_scanline(uint32_t *fb, int y);
bool write_ppm(const char *fname, uint32_t *pixels, int width, int height);
unsigned long get_msec();
int calc_subdiv(int rays);

int main(int argc, char **argv) {
	bool scene_loaded = false;

	for (int i=1; i<argc; i++) {
		// if we run with -nosdl, just render and exit
		if (strcmp(argv[i], "-nosdl") == 0) {
			use_sdl = false;
		}
		else if (strcmp(argv[i], "-size") == 0) {
			i++;
			if (!argv[i] || sscanf(argv[i], "%dx%d", &width, &height) < 2) {
				fprintf(stderr, "-size should be followed by WxH\n");
				return 1;
			}
		}
		else if (strcmp(argv[i], "-rays") == 0) {
			if (!argv[++i] || !isdigit(argv[i][0])) {
				fprintf(stderr, "-rays should be followed by the number of rays per pixel\n");
				return 1;
			}
			rays_ppxl = atoi(argv[i]);
		}
		else if (strcmp(argv[i], "-gamma") == 0) {
			if (!argv[++i] || !isdigit(argv[i][0])) {
				fprintf(stderr, "-gamma should be followrd by the desired output gamma\n");
				return 1;
			}
			inv_gamma = 1.0 / atof(argv[i]);
		}
		else {
			if (!scene.load(argv[i])) {
				fprintf(stderr, "failed to load scene file: %s\n", argv[i]);
				return 1;
			}
			scene_loaded = true;
		}
	}

	pix_subdiv = calc_subdiv(rays_ppxl);
	printf("rays: %d  ->  subdiv: %d\n", rays_ppxl, pix_subdiv);

	if (!scene_loaded) {
		fprintf(stderr, "must specify a scene file\n");
		return 1;
	}

	if (use_sdl) {
		SDL_Init(SDL_INIT_VIDEO);
	
		if (!(fbsurf = SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE))) {
			fprintf(stderr, "set video mode failed\n");
			return 1;
		}
		SDL_WM_SetCaption("Eleni's Path Tracer", 0);
	}

	// allocate framebuffer
	image = new uint32_t[width * height];
	memset(image, 0x0f, width * height * sizeof *image);

	unsigned long start = get_msec();

	if(!use_sdl) {
		render();

		unsigned long msec = get_msec() - start;
		printf("rendering completed in %lu msec\n", msec);

		cleanup();
		return 0;
	}

	int next_scanline = 0;
	
	for(;;) {
		SDL_Event ev;

		while(SDL_PollEvent(&ev)) {
			switch(ev.type) {
			case SDL_QUIT:
				cleanup();
				return 0;

			case SDL_KEYDOWN:
				switch(ev.key.keysym.sym) {
				case SDLK_ESCAPE:
					cleanup();
					return 0;

				case 's':
				case 'S':
					printf("saving image\n");
					if(!write_ppm("out.ppm", image, width, height)) {
						fprintf(stderr, "failed to save image\n");
					}
					break;

				default:
					break;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				printf("mouse click: %d %d\n", ev.button.x, ev.button.y);
				break;

			default:
				break;
			}
		}

		if(next_scanline < height) {
			render_scanline(image, next_scanline++);
			update();

			if(next_scanline == height) {
				unsigned long msec = get_msec() - start;
				printf("rendering completed in %lu msec\n", msec);
			}
		}
	}

	cleanup();
	return 0;
}

/* redraws the SDL framebuffer by copying in the rendered image.
 * this only gets called when rendering interactively.
 */
void update()
{
	if(SDL_MUSTLOCK(fbsurf)) {
		SDL_LockSurface(fbsurf);
	}

	uint32_t *fb = (uint32_t*)fbsurf->pixels;
	memcpy(fb, image, width * height * sizeof *fb);

	if(SDL_MUSTLOCK(fbsurf)) {
		SDL_UnlockSurface(fbsurf);
	}

	SDL_Flip(fbsurf);
}

void cleanup() {
	delete [] image;
	SDL_Quit();
}

/* this function is only called when rendering non-interactively
 * so don't bother dealing with SDL surfaces and such
 */
void render() {
	for(int y=0; y<height; y++) {
		printf(" rendering: [");
		int progr = 100 * (y + 1) / height;
		for(int i=0; i<100; i+=2) {
			if(i < progr) {
				putchar('=');
			} else if(i - progr > 1) {
				putchar(' ');
			} else {
				putchar('>');
			}
		}
		printf("] %d%%\r", progr);
		fflush(stdout);

		render_scanline(image, y);
	}

	putchar('\n');

	if(!write_ppm("out.ppm", image, width, height)) {
		fprintf(stderr, "failed to write image: out.ppm\n");
	}
}


void render_scanline(uint32_t *fb, int y) {
	fb += y * width;	// advance to the start of this scanline

	// render each pixel of this scanline
	for (int x = 0; x < width; x++) {
		double xpos = 2.0 * ((double)x + 0.5) / (double)width - 1.0;
		double ypos = 1.0 - 2.0 * ((double)y + 0.5) / (double)height;

		double pxl_width =	2.0 / (double) width;
		double pxl_height = 2.0 / (double) height;
		
		Color color = avg_color(pxl_width, pxl_height, xpos, ypos, pix_subdiv);
		color.x = pow(color.x, inv_gamma);
		color.y = pow(color.y, inv_gamma);
		color.z = pow(color.z, inv_gamma);

		int r = (int)(color.x * 255.0);
		int g = (int)(color.y * 255.0);
		int b = (int)(color.z * 255.0);

		if(r > 255) r = 255;
		if(g > 255) g = 255;
		if(b > 255) b = 255;

		fb[x] = ((uint32_t) r << 16) | ((uint32_t) g << 8) | (uint32_t) b;
	}
}

Color trace(const Ray &ray, int depth) {
	if(!depth) {
		return Color(0, 0, 0);
	}

	IntInfo min_info;
	bool isect = scene.intersection(ray, &min_info);
	if (isect) {
		return shade(ray, &min_info, depth);
	}

	return Color(0, 0, 0);
}

Color shade(const Ray &ray, IntInfo* min_info, int depth) {
	
	Vector3 n = min_info->normal;
	
	if ( dot(n, ray.dir) > 0 ) {
		n = -n;
	}

	Vector3 p = min_info->i_point;
	Vector3 v = normalize(ray.origin - p);

	const Material *mat = min_info->object->get_material();

	Color color = scene.get_ambient() * mat->kd + mat->ke;

	for (int i = 0; i < (int)scene.lights.size(); i++) {
		Object *light = scene.lights[i];
		light->ignore = true;

		Ray sray;
		sray.origin = p;
		sray.dir = light->sample() - p;

		if (!scene.intersection(sray, 0)) {
			Vector3 l = normalize(sray.dir);
			Vector3 lr = reflect(l, n); 

			double d = dot(n, l);
			if (d < 0.0) {
				d = 0;
			}

			double lrdotv = dot(lr, v);
			if(lrdotv < 0.0) {
				lrdotv = 0.0;
			}

			double s = pow(lrdotv, mat->specexp);
			Color light_color = light->get_material()->ke; 
			color = color + (d * mat->kd + s * mat->ks) * light_color;
		}

		light->ignore = false;
	}

	/* russian roulette */

	double avg_spec = (mat->ks.x + mat->ks.y + mat->ks.z) / 3;
	double avg_diff = (mat->kd.x + mat->kd.y + mat->kd.z) / 3;
	Vector3 newdir;

	double range = MAX(avg_spec + avg_diff, 1.0);
	double rnd = (double)rand() / RAND_MAX * range;

	if (rnd < avg_diff) {
		// diffuse interaction
		newdir = sample_lambert(n);
		if ((double) rand() / RAND_MAX <= lambert(newdir, n)) {
			Ray newray;
			newray.origin = p;
			newray.dir = newdir * RAY_MAG;
			color += trace(newray, depth - 1) * mat->kd / avg_diff;
		}
	}
	else if (rnd < avg_diff + avg_spec) {
		// specular interaction
		newdir = sample_phong(-ray.dir, n, mat->specexp);
		double pdf_spec = phong(newdir, -normalize(ray.dir), n, mat->specexp);
		if((double)rand() / RAND_MAX <= pdf_spec) {
			Ray newray;
			newray.origin = p;
			newray.dir = newdir * RAY_MAG;
			color += trace(newray, depth - 1) * mat->ks / avg_spec;
		}
	}

/* reflections */
/*	if (mat->kr > 0.0) {
		Ray refray;
		refray.origin = p;
		refray.dir = reflect(-ray.dir, n);
		color = color + mat->kr * trace(refray, depth-1) * mat->ks;
	}
*/
	return color;
}

bool write_ppm(const char *fname, uint32_t *pixels, int width, int height) {
	FILE *fp;

	if(!(fp = fopen(fname, "wb"))) {
		return false;
	}

	fprintf(fp, "P6\n%d %d\n255\n", width, height);

	int imgsz = width * height;
	for(int i=0; i<imgsz; i++) {
		uint32_t pix = *pixels++;
		int r = (pix >> 16) & 0xff;
		int g = (pix >> 8) & 0xff;
		int b = pix & 0xff;

		fputc(r, fp);
		fputc(g, fp);
		fputc(b, fp);
	}
	fclose(fp);
	return true;
}

Color avg_color(double pxl_width, double pxl_height, double x, double y, int depth) {
	Color c;
	double w = pxl_width;
	double h = pxl_height;	

	if (depth == 0) {
		if(rays_ppxl > 1) {
			x += (double) rand() / RAND_MAX * pxl_width - pxl_width / 2.0;
			y += (double) rand() / RAND_MAX * pxl_height - pxl_height / 2.0;
		}
		Ray ray = scene.get_camera()->get_primary_ray(x, y);
		c = trace(ray, MAX_DEPTH);
		c.x = c.x > 1.0 ? 1.0 : c.x;
		c.y = c.y > 1.0 ? 1.0 : c.y;
		c.z = c.z > 1.0 ? 1.0 : c.z;
		return c;
	}
	c = c + avg_color(w/2, h/2, x + w/4, y + h/4, depth-1);
	c = c + avg_color(w/2, h/2, x + w/4, y - h/4, depth-1);
	c = c + avg_color(w/2, h/2, x - w/4, y + h/4, depth-1);
	c = c + avg_color(w/2, h/2, x - w/4, y - h/4, depth-1);
	c = c/4;
	return c;
}

#if defined(unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/time.h>

unsigned long get_msec() {
	struct timeval tv;
	static struct timeval tv0;

	gettimeofday(&tv, 0);

	if(tv0.tv_sec == 0 && tv0.tv_usec == 0) {
		tv0 = tv;
		return 0;
	}

	return (tv.tv_sec - tv0.tv_sec) * 1000 + (tv.tv_usec - tv0.tv_usec) / 1000;
}

#elif defined(WIN32) || defined(__WIN32__)
#include <windows.h>

unsigned long get_msec() {
	return timeGetTime();
}

#else
#error "unsupported platform"
#endif

int calc_subdiv(int rays)
{
	int sub = 0;

	while(pow(4.0, sub) <= rays) {
		sub++;
	}
	return sub - 1;
}
