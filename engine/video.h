
#ifndef SPARTOR_VIDEO_H_
#define SPARTOR_VIDEO_H_


extern TEX_T     *textures;
extern size_t     tex_count;


extern int v_drawhulls;
extern int v_showstats;
extern int v_fullscreen;
extern int v_center;

extern int v_camx;
extern int v_camy;

extern int v_eyex;
extern int v_eyey;
extern int v_eyez;

extern int v_w;
extern int v_h;


void videoinit();
void render();
void setvideo(int w,int h,int go_full,int quiet);
void setvideosoon(int w,int h,int go_full,int delay);
int  screen2native_x(int x);
int  screen2native_y(int y);
int  make_sure_texture_is_loaded(const char *texfile);

#endif

