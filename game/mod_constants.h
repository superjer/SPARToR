
#ifndef MOD_CONSTANTS_H__
#define MOD_CONSTANTS_H__


#include "constants.h"


#define NATIVEW    480
#define NATIVEH    300
#define GAMENAME   "deadking"
#define CBDATASIZE 4     //context block (CB) payload size
#define MAXCMDDATA 64    //maximum size of extra command data


#define TEX_TOOL   0
#define TEX_PLAYER 1
#define TEX_WORLD  2
#define TEX_AMIGO  3
#define TEX_PERSON 4
#define TEX_HUD    5


//obj types
enum { ENGINE_OBJT_LIST()
       OBJT_MOTHER,
       OBJT_GHOST,
       OBJT_DUMMY,
       OBJT_PLAYER,
       OBJT_PERSON,
       OBJT_BULLET,
       OBJT_SLUG,
       OBJT_AMIGO,
       OBJT_AMIGOSWORD,
       OBJT_MAX };

enum DIR8 { NODIR=0, E, NE, N, NW, W, SW, S, SE };

//character types for PERSON_t
enum CHARACTERS { CHR_AZMA,
                  CHR_SLUG };

// Amigo's states
enum { AMIGO_HELLO,
       AMIGO_COOLDOWN,
       AMIGO_JUMP,
       AMIGO_SLASH,
       AMIGO_FLYKICK,
       AMIGO_DASH };


#endif

