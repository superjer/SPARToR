/**
 **  mcdiddy's game
 **  implementation example for the spartor network game engine
 **  copyright (c) 2010-2011  jer wilson
 **
 **  see copying for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/spartor
 **/

#include "obj_.h"

void obj_mother_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  int i, j;
  int slot0;

  for(i=0;i<maxclients;i++) {
    if( !(fr[b].cmds[i].flags & CMDF_NEW) )
      continue;

    for(j=0;j<maxobjs;j++)
      if( fr[b].objs[j].type==OBJT_GHOST && ((GHOST_t *)fr[b].objs[j].data)->client==i )
        SJC_Write( "%d: Client %i already has a ghost at obj#%d!", hotfr, i, j );

    MKOBJ( gh, GHOST, OBJF_POS );
    int ghostslot = slot0;
    MKOBJ( pl, PLAYER, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_PVEL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDX|OBJF_BNDY );

    SJC_Write( "%d: New client %i created ghost is obj#%d player is obj#%d", hotfr, i, ghostslot, slot0 );

    gh->pos = (V){0.0f,0.0f,0.0f};
    gh->client = i;
    gh->avatar = slot0;

    pl->pos  = (V){(i+1)*64,-50.0f,0.0f};
    pl->vel  = (V){0.0f,0.0f,0.0f};
    pl->hull[0] = (V){-6.0f,-15.0f,0.0f};
    pl->hull[1] = (V){ 6.0f, 15.0f,0.0f};
    pl->pvel = (V){0.0f,0.0f,0.0f};
    pl->model = i%5;
    pl->ghost = ghostslot;
    pl->goingl = 0;
    pl->goingr = 0;
    pl->goingu = 0;
    pl->goingd = 0;
    pl->jumping = 0;
    pl->firing = 0;
    pl->cooldown = 0;
    pl->projectiles = 0;
    pl->grounded = 0;
    pl->facingr = 1;
    pl->stabbing = 0;
    pl->hovertime = 0;
  } //end for i<maxclients

  //create a slug every now and then
  if(hotfr%77==0) {
    MKOBJ( sl, SLUG, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDY );
    sl->pos  = (V){(hotfr%2)*368.0f+8.0f,0.0f,0.0f};
    sl->vel  = (V){(hotfr%2)?-0.5f:0.5f,0.0f,0.0f};
    sl->hull[0] = (V){-8.0f,-4.0f,0.0f};
    sl->hull[1] = (V){ 8.0f, 8.0f,0.0f};
    sl->model = 0;
    sl->dead = 0;
  }

  //creat AMIGO!
  if(hotfr==200) {
    MKOBJ( am, AMIGO, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDY );
    am->pos  = (V){250,0,0};
    am->vel  = (V){0,0,0};
    am->hull[0] = (V){-8,-18,0};
    am->hull[1] = (V){ 8, 18,0};
    am->model = 0;
    am->state = AMIGO_HELLO;
    am->statetime = 0;
  }
}

