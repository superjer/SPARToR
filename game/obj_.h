//
// This is the header for all obj_*.c files
//


#ifndef DEADKING_OBJ_H_
#define DEADKING_OBJ_H_


#include "mod.h"


//obj types
enum { OBJT_EMPTY = 0,
       OBJT_CONTEXT, //from engine
       OBJT_MOTHER,
       OBJT_GHOST,
       OBJT_DUMMY,
       OBJT_PLAYER,
       OBJT_PERSON,
       OBJT_BULLET,
       OBJT_SLUG,
       OBJT_AMIGO,
       OBJT_AMIGOSWORD };

enum DIR8 { NODIR=0, E, NE, N, NW, W, SW, S, SE };

// MOTHER //
typedef struct{
  char edit;
  int party[6];
} MOTHER_t;

void obj_mother_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );


// GHOST //
typedef struct{
  V pos;
  V vel;
  V hull[2];
  int model;
  int client;
  int avatar;
  int clipboard_x;
  int clipboard_y;
  int clipboard_z;
  CB *clipboard_data;
} GHOST_t;

void obj_ghost_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_ghost_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );


// DUMMY //
typedef struct{
  V pos;
  V vel;
  V hull[2];
  int model;
} DUMMY_t;

void obj_dummy_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_dummy_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );


// PLAYER //
typedef struct{
  V pos;
  V vel;
  V hull[2];
  V pvel;
  int model;
  int ghost;
  char goingl;
  char goingr;
  char goingu;
  char goingd;
  char jumping;
  char firing;
  char cooldown;
  char projectiles;
  char gunback,gundown;
  char grounded;
  char facingr;
  char turning;
  char stabbing;
  char hovertime;
} PLAYER_t;

void obj_player_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_player_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );


// PERSON //
typedef struct{
  V pos;
  V vel;
  V hull[2];
  V pvel;
  int model;
  int ghost;
  int tilex;
  int tilez;
  enum DIR8 dir;
  int walkcounter;
  int hp;
  int mp;
  int st;
  int ap;
  int pn;
  int ml;
  int to;
  int xp;
  int max_hp;
  int max_mp;
  int max_st;
  int max_ap;
  int max_pn;
  int max_ml;
  int max_to;
  int max_xp;
} PERSON_t;

void obj_person_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_person_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );


// BULLET //
typedef struct{
  V pos;
  V vel;
  int model;
  int owner;
  Uint8 ttl;
} BULLET_t;

void obj_bullet_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_bullet_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );


// SLUG //
typedef struct{
  V pos;
  V vel;
  V hull[2];
  int model;
  int dead;
  int spawner;
} SLUG_t;

void obj_slug_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_slug_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );


// AMIGO //
enum { AMIGO_HELLO,
       AMIGO_COOLDOWN,
       AMIGO_JUMP,
       AMIGO_SLASH,
       AMIGO_FLYKICK,
       AMIGO_DASH };

typedef struct{
  V pos;
  V vel;
  V hull[2];
  int model;
  char state;
  int statetime;
  int hatcounter;
  int sword;
  V sword_dist;
} AMIGO_t;

void obj_amigo_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_amigo_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );


// AMIGOSWORD //
typedef struct{
  V pos;
  V vel;
  V hull[2];
  int model;
  int owner;
  int spincounter;
} AMIGOSWORD_t;

void obj_amigosword_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_amigosword_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );

#endif

