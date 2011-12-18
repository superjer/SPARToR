/**
 **  McDiddy's Game
 **  Implementation example for the SPARToR Network Game Engine
 **  Copyright (c) 2010-2011  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "obj_.h"

//FIXME REMOVE! change local player model
int    setmodel = -1;
//

void obj_player_draw( int objid, OBJ_t *o )
{
  PLAYER_t *pl = o->data;
  int x = pl->pos.x-10;
  int y = pl->pos.y-15;
  int z = y + pl->hull[1].y;
  int xshift = (pl->goingd>0 ? 40 : 0) + (pl->turning ? 80 : (pl->facingr ? 0 : 20 ));

  SJGL_SetTex( sys_tex[TEX_PLAYER].num );

  if( pl->facingr ) {
    SJGL_Blit( &(SDL_Rect){xshift,pl->model*30,20,30}, x, y, z);
  } else {
    SJGL_Blit( &(SDL_Rect){xshift,pl->model*30,20,30}, x, y, z);
  }
}

void obj_player_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  int i;
  PLAYER_t *oldme = oa->data;
  PLAYER_t *newme = ob->data;
  GHOST_t *gh = fr[b].objs[newme->ghost].data;

  newme->dashing = 0;

  int cmd = fr[b].cmds[gh->client].cmd;

  if( newme->cooldown && cmd%2 ) //keydowns are odd numbers -- FIXME: WARNING: ALERT: CAUTION: HACK
    ; // no control while dashing
  else switch( cmd ) {
    case CMDT_1LEFT:  newme->goingl  = 1;
                      if( newme->facingr) newme->turning = 3;
                      newme->facingr = 0;                      break;
    case CMDT_0LEFT:  newme->goingl  = 0;                      break;
    case CMDT_1RIGHT: newme->goingr  = 1;
                      if(!newme->facingr) newme->turning = 3;
                      newme->facingr = 1;                      break;
    case CMDT_0RIGHT: newme->goingr  = 0;                      break;
    case CMDT_1UP:    newme->goingu  = 1;                      break;
    case CMDT_0UP:    newme->goingu  = 0;                      break;
    case CMDT_1DOWN:  newme->goingd  = 1;                      break;
    case CMDT_0DOWN:  newme->goingd  = 0;                      break;
    case CMDT_1JUMP:  newme->jumping = 1;                      break;
    case CMDT_0JUMP:  newme->jumping = 0;                      break;
    case CMDT_1DASH:  newme->dashing = 1;                      break;
    case CMDT_0DASH:  newme->dashing = 0;                      break;
  }

  if( !oldme ) { //FIXME why's this null?
    SJC_Write("Warning: oldme is NULL!");
    return;
  }

  gh->vel.x = newme->pos.x - gh->pos.x; //put ghost in the right spot
  gh->vel.y = newme->pos.y - gh->pos.y;

  if( ((GHOST_t *)fr[b].objs[newme->ghost].data)->client==me ) { //local client match
    v_camx = gh->pos.x;
    v_camy = gh->pos.y;
  }

  // friction
  if(      newme->vel.x> 0.2f ) newme->vel.x -= 0.2f;
  else if( newme->vel.x>-0.2f ) newme->vel.x  = 0.0f;
  else                          newme->vel.x += 0.2f;
  if(      newme->pvel.x> 0.5f ) newme->pvel.x -= 0.5f;
  else if( newme->pvel.x>-0.5f ) newme->pvel.x  = 0.0f;
  else                           newme->pvel.x += 0.5f;

  // -- WALK --
  if( newme->turning )
    newme->turning--;
  if( newme->goingl ) {
    if(      newme->pvel.x>-2.0f ) newme->pvel.x += -1.0f;
    else if( newme->pvel.x>-3.0f ) newme->pvel.x  = -3.0f;
  }
  if( newme->goingr ) {
    if(      newme->pvel.x< 2.0f ) newme->pvel.x +=  1.0f;
    else if( newme->pvel.x< 3.0f ) newme->pvel.x  =  3.0f;
  }

  // -- JUMP --
  if( newme->pvel.y <= -2.0f ) {     //jumping in progress
    newme->pvel.y   +=  2.0f;        //jumpvel fades into real velocity
    newme->vel.y    += -2.0f;
  }
  else if( newme->pvel.y < 0.0f ) {  //jumping ending
    newme->vel.y    += newme->pvel.y;
    newme->pvel.y   = 0.0f;
    newme->jumping  = 0;             //must press jump again now
  }
  if( !newme->jumping )              //low-jump, cancel jump velocity early
    newme->pvel.y   = 0.0f;
  if( (newme->vel.y==0.0f || oldme->vel.y==0.0f) && newme->jumping ) //FIXME 0 velocity means grounded? not really
    newme->pvel.y   = -9.1f;         //initiate jump!

  if( newme->cooldown )
    newme->cooldown--;

  // -- DASH --
  if( !newme->cooldown && newme->dashing ) {
    newme->pvel.x = newme->facingr ? 10 : -10;
    newme->pvel.y = 0;
    newme->vel.x = 0;
    newme->vel.y = 0;
    newme->jumping = 0;
    newme->cooldown = 10;
  }

  for(i=0;i<objid;i++)  //find other players to interact with -- who've already MOVED
    if(fr[b].objs[i].type==OBJT_PLAYER) {
      PLAYER_t *oldyou = fr[a].objs[i].data;
      PLAYER_t *newyou = fr[b].objs[i].data;
      if(    !oldyou
          || fabsf(newme->pos.x - newyou->pos.x)>5.0f //we're not on top of each other
          || fabsf(newme->pos.y - newyou->pos.y)>2.0f
          || newme->goingr  ||  newme->goingl         //or we're moving
          || newyou->goingr || newyou->goingl         )
        continue;
      if(newme->pos.x < newyou->pos.x) {
        newme->pvel.x  -= 0.2f;
        newyou->pvel.x += 0.2f;
      } else {
        newme->pvel.x  += 0.2f;
        newyou->pvel.x -= 0.2f;
      }
    }

  //gravity?
  if( !newme->cooldown )
    newme->vel.y += 0.7f;
}

