#ifndef SPARTOR_VIDEO_H_
#define SPARTOR_VIDEO_H_

#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"

extern SDL_Window *screen;
extern SDL_GLContext *glcontext;

extern TEX_T     *textures;
extern size_t     tex_count;

extern int v_drawhulls;
extern int v_showstats;
extern int v_oscillo;
extern int v_fullscreen;
extern int v_center;
extern int v_scale;

extern int v_camx;
extern int v_camy;

extern int v_eyex;
extern int v_eyey;
extern int v_eyez;

extern int v_targx;
extern int v_targy;
extern int v_targz;

extern int v_eyedist;
extern float v_fovy;

extern int v_w;
extern int v_h;

extern GLdouble v_modeltrix[16];
extern GLdouble v_projtrix[16];
extern int v_viewport[4];

extern float v_conscale;

void videoinit();
void render();
void setvideo(int w, int h, int go_full, int quiet);
void setvideosoon(int w, int h, int go_full);
void setwinpos(int x, int h);
V get_screen_ray(double x, double y);
V get_screen_pos(double x, double y, double z);
int  make_sure_texture_is_loaded(const char *texfile);

#endif
