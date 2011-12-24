
#ifndef DEADKING_MOD_H_
#define DEADKING_MOD_H_


#define NATIVEW    480
#define NATIVEH    300
#define GAMENAME   "deadking"
#define CBDATASIZE 4     //context block (CB) payload size
#define MAXCMDDATA 64    //maximum size of extra command data


#include <GL/glew.h>
#include "SDL.h"
#include "SDL_net.h"
#include "main.h"
#include "console.h"
#include "net.h"
#include "video.h"
#include "input.h"
#include "patt.h"
#include "projection.h"
#include "mod_private.h"
#include "obj_.h"
#include <math.h>


#define COUNTOF(ident) ((sizeof (ident)) / (sizeof *(ident)))


#define TEX_TOOL   0
#define TEX_PLAYER 1
#define TEX_WORLD  2
#define TEX_AMIGO  3
#define TEX_PERSON 4
#define TEX_HUD    5


extern SYS_TEX_T  sys_tex[];
extern size_t     num_sys_tex;

extern INPUTNAME_t inputnames[];
extern int numinputnames;


extern char objectnames[][16];


extern CB *hack_map; //FIXME REMOVE SOMEDAY
extern CB *hack_dmap;


void mod_setup(Uint32 setupfr);
void mod_recvobj(OBJ_t *o);
void mod_setvideo(int w,int h);
void mod_quit();
void mod_keybind(int device,int sym,int press,char cmd);
int  mod_mkcmd(FCMD_t *c,int device,int sym,int press);
int  mod_command(char *q);
void mod_loadsurfs(int quit);
void mod_predraw(Uint32 vidfr);
void mod_draw(int objid,Uint32 vidfr,OBJ_t *o);
void mod_postdraw(Uint32 vidfr);
void mod_huddraw(Uint32 vidfr);
void mod_outerdraw(Uint32 vidfr,int w,int h);
void mod_adv(int objid,Uint32 a,Uint32 b,OBJ_t *oa,OBJ_t *ob);


#endif

