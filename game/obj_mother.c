/**
 **  Dead Kings' Quest
 **  A special game for the SPARToR Network Game Engine
 **  Copyright (c) 2010-2012  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "obj_.h"

int in_party(MOTHER_t *mo,int objid);

void obj_mother_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  int i, j;
  int slot0;

  MOTHER_t *mo = ob->data;

  // if there's no active, find one!
  if( !mo->active )
    for( i=0; i<maxobjs; i++ )
      if( fr[b].objs[i].type==OBJT_PERSON ) {
        PERSON_t *pe = fr[b].objs[i].data;
        if( pe->to >= pe->max_to ) {
          mo->active = i;
          mo->turnstart = hotfr;
          mo->intervalstart = hotfr;
          mo->pc = in_party(mo,i);
          break;
        }
      }

  // if no one can be made active, make sure to go to the next interval
  if( !mo->active ) mo->intervalstart = hotfr;

  // look for a new connected player
  for(i=0;i<maxclients;i++) {
    if( !(fr[b].cmds[i].flags & CMDF_NEW) )
      continue;

    for(j=0;j<maxobjs;j++)
      if( fr[b].objs[j].type==OBJT_GHOST && ((GHOST_t *)fr[b].objs[j].data)->client==i )
        SJC_Write( "%d: Client %i already has a ghost at obj#%d!", hotfr, i, j );

    //FIXME context is hardcoded as 1 for GHOST and PLAYER:
    MKOBJ( gh, GHOST,  1, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_BNDX|OBJF_BNDZ|OBJF_BNDB|OBJF_BNDT );
    int ghostslot = slot0;
    MKOBJ( az, PERSON, 1, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDB|OBJF_BNDX|OBJF_BNDZ );
    int azslot = slot0;
    MKOBJ( gy, PERSON, 1, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDB|OBJF_BNDX|OBJF_BNDZ );
    int gyslot = slot0;
    MKOBJ( en, PERSON, 1, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDB|OBJF_BNDX|OBJF_BNDZ );

    SJC_Write( "%d: New client %i created ghost is obj#%d player is obj#%d", hotfr, i, ghostslot, azslot );

    mo->ghost    = ghostslot;
    mo->party[0] = azslot;
    mo->party[1] = gyslot;

    gh->pos            = (V){  0,  0,  0};
    gh->vel            = (V){  0,  0,  0};
    gh->hull[0]        = (V){  0,  0,  0};
    gh->hull[1]        = (V){  0,  0,  0};
    gh->model          = 0;
    gh->goingl         = 0;
    gh->goingr         = 0;
    gh->goingu         = 0;
    gh->goingd         = 0;
    gh->client         = i;
    gh->avatar         = slot0;
    gh->clipboard_x    = 0;
    gh->clipboard_y    = 0;
    gh->clipboard_data = NULL;

    az->pos         = (V){150,0,150};
    az->vel         = (V){0,0,0};
    az->hull[0]     = (V){-5,-34,-5};
    az->hull[1]     = (V){ 5,  0, 5};
    az->model       = 0;
    az->tilex       = 1;
    az->tilez       = 1;
    az->dir         = S;
    az->walkcounter = 0;
    az->character   = CHR_AZMA;
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

    gy->pos         = (V){150,0,150};
    gy->vel         = (V){0,0,0};
    gy->hull[0]     = (V){-5,-34,-5};
    gy->hull[1]     = (V){ 5,  0, 5};
    gy->model       = 0;
    gy->tilex       = 3;
    gy->tilez       = 5;
    gy->dir         = S;
    gy->walkcounter = 0;
    gy->character   = CHR_GYLLIOC;
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

    en->pos         = (V){150,0,150};
    en->vel         = (V){0,0,0};
    en->hull[0]     = (V){-5,-34,-5};
    en->hull[1]     = (V){ 5,  0, 5};
    en->model       = 0;
    en->tilex       = 5;
    en->tilez       = 5;
    en->dir         = S;
    en->walkcounter = 0;
    en->character   = CHR_SLUG;
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
  } //end for i<maxclients
}

int in_party(MOTHER_t *mo,int objid)
{
  int i;
  for( i=0; i<PARTY_SIZE; i++ )
    if( mo->party[i]==objid )
      return 1;
  return 0;
}
