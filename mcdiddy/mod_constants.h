
#ifndef MOD_CONSTANTS_H__
#define MOD_CONSTANTS_H__


#include "constants.h"


#define NATIVEW    384
#define NATIVEH    240
#define NEARVAL    (-999)
#define FARVAL     ( 999)
#define GAMENAME   "mcdiddy"

#define CBDATASIZE 4     //context block (CB) payload size
#define MAXCMDDATA 64    //maximum size of extra command data


#define TEX_TOOL   0
#define TEX_PLAYER 1
#define TEX_WORLD  2
#define TEX_AMIGO  3


#define DEPTH_OF(nativey) ((nativey) - v_camy + NATIVEH)


//obj types
enum { ENGINE_OBJT_LIST()
       OBJT_MOTHER,
       OBJT_GHOST,
       OBJT_DUMMY,
       OBJT_PLAYER,
       OBJT_BULLET,
       OBJT_SLUG,
       OBJT_AMIGO,
       OBJT_AMIGOSWORD,
       OBJT_MAX };

enum DIR8 { NODIR=0, E, NE, N, NW, W, SW, S, SE };

// Amigo's states
enum { AMIGO_HELLO,
       AMIGO_COOLDOWN,
       AMIGO_JUMP,
       AMIGO_SLASH,
       AMIGO_FLYKICK,
       AMIGO_DASH };


#endif

