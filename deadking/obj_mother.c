/**
 **  Dead Kings' Quest
 **  A special game for the SPARToR Network Game Engine
 **  Copyright (c) 2010-2015  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "obj_.h"

int in_party(MOTHER_t *mo, int objid);
void init_new_player(MOTHER_t *mo, int client_nr, Uint32 b);

void obj_mother_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  int i;
  MOTHER_t *mo = ob->data;

  // if there's no active, find one!
  if( !mo->active )
    for( i=0; i<maxobjs; i++ )
      if( fr[b].objs[i].type==OBJT_PERSON )
      {
        PERSON_t *pe = fr[b].objs[i].data;
        if( pe->to >= pe->max_to )
        {
          mo->active = i;
          mo->turnstart = hotfr;
          mo->intervalstart = hotfr;
          mo->pc = in_party(mo, i);

          if( mo->pc )
            mo->menulayer = MAIN;
          else
            mo->menulayer = NOLAYER;

          break;
        }
      }

  for( i=0; i<maxobjs; i++ )
    if( fr[b].objs[i].type==OBJT_POPUP )
    {
      POPUP_t *pop = fr[b].objs[i].data;
      pop->visible = (pop->layer == mo->menulayer);
    }

  // if no one can be made active, make sure to go to the next interval
  if( !mo->active ) mo->intervalstart = hotfr;

  // look for a new connected player
  for( i=0; i<maxclients; i++ )
    if( fr[b].cmds[i].flags & CMDF_NEW )
      init_new_player(mo, i, b);
}

void init_new_player(MOTHER_t *mo, int client_nr, Uint32 b)
{
  int j;
  int slot0;

  for( j=0; j<maxobjs; j++ )
    if( fr[b].objs[j].type==OBJT_GHOST && ((GHOST_t *)fr[b].objs[j].data)->client==client_nr )
      echo( "%d: Client %i already has a ghost at obj#%d!", hotfr, client_nr, j );

  #define PERS_FLAGS OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDB|OBJF_BNDX|OBJF_BNDZ

  //FIXME context is hardcoded as 1 for these things
  MKOBJ( gh, GHOST,  1, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_BNDX|OBJF_BNDZ|OBJF_BNDB|OBJF_BNDT );
  int ghostslot = slot0;
  MKOBJ( az, PERSON, 1, PERS_FLAGS );
  int azslot = slot0;
  MKOBJ( gy, PERSON, 1, PERS_FLAGS );
  int gyslot = slot0;
  MKOBJ( en, PERSON, 1, PERS_FLAGS );
  //int enslot = slot0;

  echo( "%d: New client %i created ghost is obj#%d player is obj#%d", hotfr, client_nr, ghostslot, azslot );

  mo->ghost    = ghostslot;
  mo->party[0] = azslot;
  mo->party[1] = gyslot;

  memset(gh, 0, sizeof *gh);
  gh->client      = client_nr;
  gh->avatar      = gyslot;
  gh->pos         = (V){340, 0, 340};

  memset(az, 0, sizeof *az);
  az->pos         = (V){150, 0, 150};
  az->vel         = (V){0, 0, 0};
  az->hull[0]     = (V){-5, -34, -5};
  az->hull[1]     = (V){ 5,  0, 5};
  az->tilex       = 11;
  az->tilez       = 11;
  az->dir         = S;
  az->gait        = RUNNING;
  az->character   = CHR_AZMA;
  az->name        = "Azmagelle";
  az->armed       = 1;
  az->hp          = 77;
  az->mp          = 100;
  az->st          = 50;
  az->ap          = 32;
  az->pn          = 0;
  az->ml          = 0;
  az->to          = 100;
  az->xp          = 3;
  az->max_hp      = 100;
  az->max_mp      = 100;
  az->max_st      = 100;
  az->max_ap      = 100;
  az->max_pn      = 100;
  az->max_ml      = 100;
  az->max_to      = 100;
  az->max_xp      = 100;

  memset(gy, 0, sizeof *gy);
  gy->pos         = (V){150, 0, 150};
  gy->vel         = (V){0, 0, 0};
  gy->hull[0]     = (V){-5, -34, -5};
  gy->hull[1]     = (V){ 5,  0, 5};
  gy->tilex       = 13;
  gy->tilez       = 15;
  gy->dir         = S;
  gy->gait        = RUNNING;
  gy->character   = CHR_GYLLIOC;
  gy->name        = "Gyllioc";
  gy->armed       = 1;
  gy->hp          = 99;
  gy->mp          = 67;
  gy->st          = 50;
  gy->ap          = 38;
  gy->pn          = 1;
  gy->ml          = 1;
  gy->to          = 80;
  gy->xp          = 3;
  gy->max_hp      = 100;
  gy->max_mp      = 100;
  gy->max_st      = 100;
  gy->max_ap      = 100;
  gy->max_pn      = 100;
  gy->max_ml      = 100;
  gy->max_to      = 100;
  gy->max_xp      = 100;

  memset(en, 0, sizeof *en);
  en->pos         = (V){150, 0, 150};
  en->hull[0]     = (V){-5, -34, -5};
  en->hull[1]     = (V){ 5,  0, 5};
  en->tilex       = 15;
  en->tilez       = 15;
  en->dir         = S;
  en->gait        = RUNNING;
  en->character   = CHR_SLUG;
  en->name        = "Sluggathor";
  en->armed       = 1;
  en->hp          = 1;
  en->mp          = 1;
  en->st          = 1;
  en->ap          = 1;
  en->pn          = 1;
  en->ml          = 1;
  en->to          = 0;
  en->xp          = 1;
  en->max_hp      = 10;
  en->max_mp      = 10;
  en->max_st      = 10;
  en->max_ap      = 14;
  en->max_pn      = 10;
  en->max_ml      = 10;
  en->max_to      = 100;
  en->max_xp      = 10;

  #define MKMENU(text_, ypos, layer_)              \
    do {                                           \
      MKOBJ( button, POPUP, 0, 0 );                \
      button->pos     = (V){NATIVEW-62, ypos, 0};  \
      button->hull[0] = (V){0, 0, 0};              \
      button->hull[1] = (V){50, 18, 0};            \
      button->visible = 0;                         \
      button->enabled = 1;                         \
      button->active  = 0;                         \
      button->layer   = layer_;                    \
      button->click   = NULL;                      \
      button->text    = text_;                     \
    } while(0)
  MKMENU("MOVE"   ,  10, MAIN);
  MKMENU("ATTACK" ,  30, MAIN);
  MKMENU("SPECIAL",  50, MAIN);
  MKMENU("MAGIC"  ,  70, MAIN);
  MKMENU("ITEM"   ,  90, MAIN);
  MKMENU("ORDERS" , 110, MAIN);
  MKMENU("STATUS" , 130, MAIN);

  MKMENU("WALK"   ,  10, MOVE);
  MKMENU("RUN"    ,  30, MOVE);
  MKMENU("SPRINT" ,  50, MOVE);
  #undef MKMENU
}

int in_party(MOTHER_t *mo, int objid)
{
  int i;
  for( i=0; i<PARTY_SIZE; i++ )
    if( mo->party[i]==objid )
      return 1;
  return 0;
}
