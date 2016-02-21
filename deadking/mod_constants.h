#ifndef MOD_CONSTANTS_H__
#define MOD_CONSTANTS_H__

#define NATIVEW    480
#define NATIVEH    300
#define NEARVAL    (-99999)
#define FARVAL     ( 99999)
#define GAMENAME   "deadking"
#define CBDATASIZE 4     //context block (CB) payload size
#define MAXCMDDATA 64    //maximum size of extra command data

#define TEX_TOOL   0
#define TEX_PLAYER 1
#define TEX_PERSON 2
#define TEX_HUD    3

#define PARTY_SIZE 6

enum DIR8 { NODIR=0, E, NE, N, NW, W, SW, S, SE };

enum MENULAYER { NOLAYER=0, MAIN, MOVE };

enum GAIT { NOGAIT=0, WALKING, RUNNING, SPRINTING };

//character types for PERSON_t
enum CHARACTERS {
        CHR_AZMA,
        CHR_GYLLIOC,
        CHR_SLUG
};

#endif
