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

#define DASH_COOLDOWN 30
#define FLAIL_COOLDOWN 20

#define MAX_WALK 2.6f

//FIXME REMOVE! hack
int smack = 0;
//

void obj_player_draw( int objid, OBJ_t *o )
{
  PLAYER_t *pl = o->data;
  int x = pl->pos.x-10;
  int y = pl->pos.y-15;
  int z = y + pl->hull[1].y;
  int xshift = (pl->goingd>0 ? 40 : 0) + (pl->turning ? 80 : (pl->facingr ? 0 : 20 ));
  int flailshift = ((pl->cooldown % 6) / 3) * 30 + 60;
  int yshift = pl->cooldown > FLAIL_COOLDOWN ? 30 : (pl->cooldown > 0 ? flailshift : 0);

  SJGL_SetTex( sys_tex[TEX_PLAYER].num );

  if( smack )
    SJGL_Blit( &(SDL_Rect){236,0,20,36}, x+smack*10, y-3, z);

  if( pl->facingr ) {
    SJGL_Blit( &(SDL_Rect){xshift,yshift,20,30}, x, y, z);
  } else {
    SJGL_Blit( &(SDL_Rect){xshift,yshift,20,30}, x, y, z);
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

  switch( cmd ) {
    case CMDT_1LEFT:  newme->goingl  = 1; break;
    case CMDT_0LEFT:  newme->goingl  = 0; break;
    case CMDT_1RIGHT: newme->goingr  = 1; break;
    case CMDT_0RIGHT: newme->goingr  = 0; break;
    case CMDT_1UP:    newme->goingu  = 1; break;
    case CMDT_0UP:    newme->goingu  = 0; break;
    case CMDT_1DOWN:  newme->goingd  = 1; break;
    case CMDT_0DOWN:  newme->goingd  = 0; break;
    case CMDT_1JUMP:  newme->jumping = 1; break;
    case CMDT_0JUMP:  newme->jumping = 0; break;
    case CMDT_1DASH:  newme->dashing = 1; break;
    case CMDT_0DASH:  newme->dashing = 0; break;
  }

  if( !oldme ) { //FIXME why's this null?
    SJC_Write("Warning: oldme is NULL!");
    return;
  }

  if( newme->goingl && !newme->cooldown ) {
    if( newme->facingr) newme->turning = 3;
    newme->facingr = 0;
  }

  if( newme->goingr && !newme->cooldown ) {
    if(!newme->facingr) newme->turning = 3;
    newme->facingr = 1;
  }

  // cancel most inputs when busy
  int busy = newme->cooldown > FLAIL_COOLDOWN;

  // put ghost in the right spot
  gh->vel.x = newme->pos.x - gh->pos.x;
  gh->vel.y = newme->pos.y - gh->pos.y;

  // check if this player is that of the local client
  if( ((GHOST_t *)fr[b].objs[newme->ghost].data)->client==me ) {
    v_camx = gh->pos.x;
    v_camy = gh->pos.y;
  }

  // -- HIT WALL? --
  smack = 0;
  if( newme->vel.x==0 ) {
    if( oldme->vel.x < -4 )
      smack = -1;
    else if( oldme->vel.x > 4 )
      smack = 1;
    if( newme->cooldown > FLAIL_COOLDOWN )
    newme->cooldown = FLAIL_COOLDOWN;
  }

  // -- FRICTION --
  if(      newme->vel.x> 0.35f ) newme->vel.x -= 0.35f;
  else if( newme->vel.x>-0.35f ) newme->vel.x  = 0.0f;
  else                           newme->vel.x += 0.35f;

  if(      newme->pvel.x> 0.35f ) newme->pvel.x -= 0.35f;
  else if( newme->pvel.x>-0.35f ) newme->pvel.x  = 0.0f;
  else                            newme->pvel.x += 0.35f;

  // -- WALK --
  if( newme->turning )
    newme->turning--;

  int cantwalk = newme->cooldown && newme->cooldown<FLAIL_COOLDOWN && newme->vel.y == 0.0;

  // throw things
  for( i = newme->beholden; i; ) {
    cantwalk = 1;
    if(   fr[b].objs[i].type == OBJT_SLUG
       || fr[b].objs[i].type == OBJT_DUMMY ) {

      V *youvel = flex( fr[b].objs+i, OBJF_VEL );

      if( !youvel ) break;
      else if( newme->goingu && !oldme->goingu ) { youvel->y = -10.0f;                     }
      else if( newme->goingr && !oldme->goingr ) { youvel->y =  -5.0f; youvel->x =  12.0f; }
      else if( newme->goingd && !oldme->goingd ) { youvel->y =  10.0f;                     }
      else if( newme->goingl && !oldme->goingl ) { youvel->y =  -5.0f; youvel->x = -12.0f; }
      else break;

      youvel->y += newme->vel.y;
    }
    newme->beholden = 0;
    break;
  }

  if( !busy && newme->goingl && !cantwalk ) {
    newme->pvel.x += -1;
    if( newme->pvel.x < -MAX_WALK )
      newme->pvel.x = -MAX_WALK;
  }

  if( !busy && newme->goingr && !cantwalk ) {
    newme->pvel.x +=  1;
    if( newme->pvel.x >  MAX_WALK )
      newme->pvel.x =  MAX_WALK;
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

  int cool_enough = ( (newme->cooldown<FLAIL_COOLDOWN && newme->grounded) || !newme->cooldown );

  if( newme->jumping &&
      (newme->vel.y==0.0f || oldme->vel.y==0.0f) &&
      cool_enough                                   ) {
    newme->pvel.y   = -7.5f;         //initiate jump!
    newme->cooldown = 0;
  }

  // -- COOLDOWN & GROUNDEDNESS --
  if( newme->cooldown )
    newme->cooldown--;

  if( newme->cooldown == FLAIL_COOLDOWN-2 && newme->vel.y==0 )
    newme->grounded = 1;

  if( newme->vel.y!=0 || !newme->cooldown )
    newme->grounded = 0;

  // -- DASH --
  if( !newme->cooldown && newme->dashing && !newme->beholden ) {
    newme->vel.x = newme->facingr ? 10 : -10;
    newme->vel.y = 0;
    newme->pvel.x = 0;
    newme->pvel.y = 0;
    newme->jumping = 0;
    newme->grounded = 0;
    newme->cooldown = DASH_COOLDOWN;
  }

  // flail speed
  if( newme->cooldown && newme->cooldown <= FLAIL_COOLDOWN ) {
    if( newme->vel.x < 0 )
      newme->vel.x = -MAX_WALK;
    else if( newme->vel.x > 0 )
      newme->vel.x =  MAX_WALK;
  }

  V clawpos = newme->pos;
  clawpos.y += 1.0f;
  if( newme->facingr ) clawpos.x += 5.0f;
  else                 clawpos.x -= 5.0f;

  // -- INTERACTION --
  for(i=0;i<maxobjs;i++)
    if(   fr[b].objs[i].type==OBJT_SLUG
       || fr[b].objs[i].type==OBJT_DUMMY ) {

      V *youpos = flex(fr[b].objs+i, OBJF_POS);

      if(    !fr[a].objs[i].data
          || !youpos
          || newme->cooldown < FLAIL_COOLDOWN
          || fabsf(clawpos.x - youpos->x)>10.0f //we're not on top of each other
          || fabsf(clawpos.y - youpos->y)>15.0f )
        continue;
      newme->beholden = i;
      newme->cooldown = 0;
      // slow player down!
      if( fabsf(newme->vel.x) > 2.0f ) newme->vel.x = (newme->vel.x>0.0 ? 2.0f : -2.0f);
    }

  // relocate beholden enemy to claw every frame
  i = newme->beholden;
  if( i ) {
    if(   fr[b].objs[i].type==OBJT_SLUG
       || fr[b].objs[i].type==OBJT_DUMMY ) {

      V *youpos = flex(fr[b].objs+i, OBJF_POS);

      if( youpos ) *youpos = clawpos;
    }
  }

  // -- GRAVITY --
  if( newme->cooldown < FLAIL_COOLDOWN )
    newme->vel.y += 0.500001f;
}

