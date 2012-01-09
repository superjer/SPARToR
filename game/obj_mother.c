/**
 **  Dead Kings' Quest
 **  A special game for the SPARToR Network Game Engine
 **  Copyright (c) 2010-2011  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "obj_.h"

void obj_mother_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  int i, j;
  int slot0;

  MOTHER_t *mo = ob->data;

  for(i=0;i<maxclients;i++) {
    if( !(fr[b].cmds[i].flags & CMDF_NEW) )
      continue;

    for(j=0;j<maxobjs;j++)
      if( fr[b].objs[j].type==OBJT_GHOST && ((GHOST_t *)fr[b].objs[j].data)->client==i )
        SJC_Write( "%d: Client %i already has a ghost at obj#%d!", hotfr, i, j );

    //FIXME context is hardcoded as 1 for GHOST and PLAYER:
    MKOBJ( gh, GHOST,  1, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_BNDX|OBJF_BNDZ|OBJF_BNDB|OBJF_BNDT );
    int ghostslot = slot0;
    MKOBJ( pl, PLAYER, 1, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_BNDX|OBJF_BNDZ|OBJF_BNDB|OBJF_PVEL|OBJF_PLAT|OBJF_CLIP );
    MKOBJ( pe, PERSON, 1, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDB|OBJF_BNDX|OBJF_BNDZ );

    SJC_Write( "%d: New client %i created ghost is obj#%d player is obj#%d", hotfr, i, ghostslot, slot0 );

    gh->pos            = (V){  0,  0,  0};
    gh->vel            = (V){  0,  0,  0};
    gh->hull[0]        = (V){  0,  0,  0};
    gh->hull[1]        = (V){  0,  0,  0};
    gh->model          = 0;
    gh->client         = i;
    gh->avatar         = slot0;
    gh->clipboard_x    = 0;
    gh->clipboard_y    = 0;
    gh->clipboard_data = NULL;

    pl->pos            = (V){ 0,-50, 0};
    pl->vel            = (V){ 0,  0, 0};
    pl->hull[0]        = (V){-6,-30,-6};
    pl->hull[1]        = (V){ 6,  0, 6};
    pl->pvel           = (V){ 0,  0, 0};
    pl->model          = i%5;
    pl->ghost          = ghostslot;
    pl->goingl         = 0;
    pl->goingr         = 0;
    pl->goingu         = 0;
    pl->goingd         = 0;
    pl->jumping        = 0;
    pl->firing         = 0;
    pl->cooldown       = 0;
    pl->projectiles    = 0;
    pl->grounded       = 0;
    pl->facingr        = 1;
    pl->turning        = 0;
    pl->stabbing       = 0;
    pl->hovertime      = 0;

    pe->pos         = (V){150,0,150};
    pe->vel         = (V){0,0,0};
    pe->hull[0]     = (V){-5,-34,-5};
    pe->hull[1]     = (V){ 5,  0, 5};
    pe->model       = 0;
    pe->ghost       = ghostslot;
    pe->tilex       = 1;
    pe->tilez       = 1;
    pe->dir         = S;
    pe->walkcounter = 0;
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
    pe->max_ap      =  32;
    pe->max_pn      = 100;
    pe->max_ml      = 100;
    pe->max_to      = 100;
    pe->max_xp      = 100;

    mo->party[0] = slot0;
  } //end for i<maxclients

  //create a slug every now and then
  if(hotfr%77==0) {
    MKOBJ( sl, SLUG, 1, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDB|OBJF_BNDX|OBJF_BNDZ );
    sl->pos     = (V){(hotfr%2)*368+8,0,0};
    sl->vel     = (V){(hotfr%2)?-0.5f:0.5f,0,0};
    sl->hull[0] = (V){-8,-4,-8};
    sl->hull[1] = (V){ 8, 8, 8};
    sl->model   = 0;
    sl->dead    = 0;
  }

  //create AMIGO!
  if(hotfr==200) {
    MKOBJ( am, AMIGO, 1, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDB|OBJF_BNDZ );
    am->pos       = (V){250,0,0};
    am->vel       = (V){0,0,0};
    am->hull[0]   = (V){-8,-18,-8};
    am->hull[1]   = (V){ 8, 18, 8};
    am->model     = 0;
    am->state     = AMIGO_HELLO;
    am->statetime = 0;
  }
}

