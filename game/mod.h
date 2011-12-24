
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
#include "mod_private.h"
#include "obj_.h"
#include <math.h>


#define COUNTOF(ident) ((sizeof (ident)) / (sizeof *(ident)))

// num tiles in a tex
#define TILEX 11
#define TILEY 21

// macros for translating to/from isometric:
#ifdef ISOMETRIC
  #define TILE2NATIVE_X(co,x,y,z) (((x)-(z))*(co)->tileuw/2      )
  #define TILE2NATIVE_Y(co,x,y,z) (((x)+(z))*(co)->tileuh/2 + (y))
  #define POINT2NATIVE_X(o)       (((o).x-(o).z)          )
  #define POINT2NATIVE_Y(o)       (((o).x+(o).z)/2 + (o).y)
  #define ISO_SHIFTX(co,x)        ((x) - (co)->tileuw/2)
  #define ISO_SHIFTY(co,y)        ((y)                 )
  #define NATIVE2TILE_X(co,x,y)   ((ISO_SHIFTX(co,x)+ISO_SHIFTY(co,y)*2)/(co)->tileuw) // times 2 b/c tiles are twice as wide as tall
  #define NATIVE2TILE_Y(co,x,y)   0
  #define NATIVE2TILE_Z(co,x,y)   ((ISO_SHIFTY(co,y)-ISO_SHIFTX(co,x)/2)/(co)->tileuh)
#else
  #define TILE2NATIVE_X(co,x,y,z) ((x)*(co)->tileuw)
  #define TILE2NATIVE_Y(co,x,y,z) ((y)*(co)->tileuh)
  #define POINT2NATIVE_X(o)       ((o).x)
  #define POINT2NATIVE_Y(o)       ((o).y)
  #define NATIVE2TILE_X(co,x,y)   ((x)/(co)->tileuw)
  #define NATIVE2TILE_Y(co,x,y)   ((y)/(co)->tileuh)
  #define NATIVE2TILE_Z(co,x,y)   0
#endif

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

