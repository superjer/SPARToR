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
#include "obj_.h"
#include "sprite.h"
#include <math.h>

extern SYS_TEX_T  sys_tex[];
extern size_t     num_sys_tex;

extern INPUTNAME_t inputnames[];
extern int numinputnames;

extern CB *hack_map; //FIXME REMOVE SOMEDAY
extern CB *hack_dmap;

void mod_setup(Uint32 setupfr);
void mod_recvobj(OBJ_t *o);
void mod_setvideo(int w, int h);
void mod_quit();
void mod_showbinds();
void mod_keybind(int device, int sym, int press, char cmd, char *script);
int  mod_mkcmd(FCMD_t *c, int device, int sym, int press);
int  mod_command(char *q, char *args);
void mod_loadsurfs(int quit);
void mod_predraw(Uint32 vidfr);
void mod_draw(int objid, Uint32 vidfr, OBJ_t *o);
void mod_postdraw(Uint32 vidfr);
void mod_huddraw(Uint32 vidfr);
void mod_outerdraw(Uint32 vidfr, int w, int h);
void mod_adv(int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob);

#endif
