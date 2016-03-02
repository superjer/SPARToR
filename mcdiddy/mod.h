#ifndef MOD_H_
#define MOD_H_

#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"
#include "SDL_net.h"
#include "mod_constants.h"
#include "main.h"
#include "console.h"
#include "pack.h"
#include "video.h"
#include "input.h"
#include "patt.h"
#include "mod_private.h"
#include "sprite.h"
#include <math.h>

extern SYS_TEX_T  sys_tex[];
extern size_t     num_sys_tex;

extern INPUTNAME inputnames[];
extern int numinputnames;

extern CB *hack_map; //FIXME REMOVE SOMEDAY
extern CB *hack_dmap;

void mod_setup(unsigned int setupfr);
void mod_recvobj(object *o);
void mod_setvideo(int w, int h);
void mod_quit();
void mod_showbinds();
void mod_keybind(int device, int sym, int press, char cmd, char *script);
int  mod_mkcmd(FCMD_t *c, int device, int sym, int press);
int  mod_command(char *q, char *args);
void mod_loadsurfs(int quit);
void mod_predraw(unsigned int vidfr);
void mod_postdraw(unsigned int vidfr);
void mod_huddraw(unsigned int vidfr);
void mod_outerdraw(unsigned int vidfr, int w, int h);

#endif
