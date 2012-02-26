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
    MKOBJ( pe, PERSON, 1, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDB|OBJF_BNDX|OBJF_BNDZ );
    int peslot = slot0;
    MKOBJ( en, PERSON, 1, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDB|OBJF_BNDX|OBJF_BNDZ );

    SJC_Write( "%d: New client %i created ghost is obj#%d player is obj#%d", hotfr, i, ghostslot, peslot );

    mo->ghost    = ghostslot;
    mo->party[0] = peslot;

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

    pe->pos         = (V){150,0,150};
    pe->vel         = (V){0,0,0};
    pe->hull[0]     = (V){-5,-34,-5};
    pe->hull[1]     = (V){ 5,  0, 5};
    pe->model       = 0;
    pe->tilex       = 1;
    pe->tilez       = 1;
    pe->dir         = S;
    pe->walkcounter = 0;
    pe->character   = CHR_AZMA;
    pe->hp          = 77;
    pe->mp          = 100;
    pe->st          = 50;
    pe->ap          = 32;
    pe->pn          = 0;
    pe->ml          = 0;
    pe->to          = 100;
    pe->xp          = 3;
    pe->max_hp      = 100;
    pe->max_mp      = 100;
    pe->max_st      = 100;
    pe->max_ap      = 100;
    pe->max_pn      = 100;
    pe->max_ml      = 100;
    pe->max_to      = 100;
    pe->max_xp      = 100;

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
