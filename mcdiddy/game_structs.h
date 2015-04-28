// DO NOT USE INCLUDE GUARDS HERE


#define TYPE MOTHER
STRUCT()
  HIDE(
    char edit;
  )
#include "endstruct.h"

#define TYPE GHOST
STRUCT()
  EXPOSE(V, pos, )
  EXPOSE(V, vel, )
  EXPOSE(V, hull, [2])
  EXPOSE(int, model, )
  HIDE(
    int client;
    int avatar;
    int clipboard_x;
    int clipboard_y;
    int clipboard_z;
    CB *clipboard_data;
  )
#include "endstruct.h"

#define TYPE DUMMY
STRUCT()
  EXPOSE(V, pos, )
  EXPOSE(V, vel, )
  EXPOSE(V, hull, [2])
  EXPOSE(int, model, )
#include "endstruct.h"

#define TYPE PLAYER
STRUCT()
  EXPOSE(V, pos, )
  EXPOSE(V, vel, )
  EXPOSE(V, hull, [2])
  EXPOSE(V, pvel, )
  EXPOSE(int, model, )
  HIDE(
    int ghost;
    char goingl;
    char goingr;
    char goingu;
    char goingd;
    char jumping;
    char firing;
    char cooldown;
    char projectiles;
    char gunback;
    char gundown;
    char grounded;
    char facingr;
    char turning;
    char stabbing;
    char hovertime;
    char walkcounter;
  )
#include "endstruct.h"

#define TYPE BULLET
STRUCT()
  EXPOSE(V, pos, )
  EXPOSE(V, vel, )
  EXPOSE(V, hull, [2])
  EXPOSE(int, model, )
  HIDE(
    int owner;
    Uint8 ttl;
    char dead;
  )
#include "endstruct.h"

#define TYPE SLUG
STRUCT()
  EXPOSE(V, pos, )
  EXPOSE(V, vel, )
  EXPOSE(V, hull, [2])
  EXPOSE(int, model, )
  HIDE(
    int dead;
    int spawner;
  )
#include "endstruct.h"

#define TYPE AMIGO
STRUCT()
  EXPOSE(V, pos, )
  EXPOSE(V, vel, )
  EXPOSE(V, hull, [2])
  EXPOSE(int, model, )
  HIDE(
    char state;
    int statetime;
    int hatcounter;
    int sword;
    V sword_dist;
  )
#include "endstruct.h"

#define TYPE AMIGOSWORD
STRUCT()
  EXPOSE(V, pos, )
  EXPOSE(V, vel, )
  EXPOSE(V, hull, [2])
  EXPOSE(int, model, )
  HIDE(
    int owner;
    int spincounter;
  )
#include "endstruct.h"

