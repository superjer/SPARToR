// DO NOT USE INCLUDE GUARDS HERE


#define TYPE MOTHER
STRUCT()
  HIDE(
    char edit;         // whether edit mode is on
    int party[6];      // objects in the players party (probably PERSONs)
    int active;        // the active, currently moving, object
    int pc;            // if the active is player controlled
    unsigned int intervalstart; // frame number the current time interval started on
    unsigned int turnstart;     // frame number the turn started on
  )
#include "endstruct.h"

#define TYPE GHOST
STRUCT()
  EXPOSE(V,pos,)
  EXPOSE(V,vel,)
  EXPOSE(V,hull,[2])
  EXPOSE(int,model,)
  HIDE(
    int goingl;
    int goingr;
    int goingu;
    int goingd;
    int client;
    int avatar;
    int clipboard_x;
    int clipboard_y;
    int clipboard_z;
    CB *clipboard_data;
  )
#include "endstruct.h"

#define TYPE PERSON
STRUCT()
  EXPOSE(V,pos,)
  EXPOSE(V,vel,)
  EXPOSE(V,hull,[2])
  EXPOSE(V,pvel,)
  EXPOSE(int,model,)
  HIDE(
    int ghost;
    int tilex;
    int tilez;
    enum DIR8 dir;
    int walkcounter;
    int character;
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
  )
#include "endstruct.h"

