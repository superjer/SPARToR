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

//FIXME REMOVE! change local player model
int    setmodel = -1;
//

void obj_player_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co )
{
  PLAYER_t *pl = o->data;
  int gunshift = pl->goingu ? 96 : pl->goingd ? 48 : 0;
  if( pl->goingu==pl->goingd ) gunshift = 0;
  int c = POINT2NATIVE_X(&pl->pos) - 10;
  int d = POINT2NATIVE_Y(&pl->pos) - 15;
  int r = d + 30;
  int ushift = (pl->goingd>0 ? 40 : 0) + (pl->turning ? 80 : (pl->facingr ? 0 : 20 ));

  SJGL_SetTex( sys_tex[TEX_PLAYER].num );

  if( pl->facingr ) {
    if( pl->model==4 ) //girl hair
      SJGL_Blit( &(REC){80,120,20,15}, c-4, d+(pl->goingd?4:0)+pl->gundown/7, r );

    SJGL_Blit( &(REC){ushift,pl->model*30,20,30}, c, d, r);

    if( !pl->stabbing ) //gun
      SJGL_Blit( &(REC){ 0+gunshift,150,24,27}, c+5-pl->gunback, d+5+pl->gundown/5, r );
  } else {
    if( pl->model==4 ) //girl hair
      SJGL_Blit( &(REC){100,120,20,15}, c+4, d+(pl->goingd?4:0)+pl->gundown/7, r );

    SJGL_Blit( &(REC){ushift,pl->model*30,20,30}, c, d, r);

    if( !pl->stabbing ) //gun
      SJGL_Blit( &(REC){24+gunshift,150,24,27}, c-9+pl->gunback, d+5+pl->gundown/5, r );
  }

  if( pl->stabbing ) //up/down stabbing
    SJGL_Blit( &(REC){148+(pl->stabbing<0?5:0),150,5,27}, c+8, d-(pl->stabbing<0?15:-15), r );

  // draw shadow
  V shadow = (V){pl->pos.x, co->bsy*co->y, pl->pos.z};
  int sc = POINT2NATIVE_X(&shadow) - 10;
  int sd = POINT2NATIVE_Y(&shadow) - 5;
  SJGL_SetTex( sys_tex[TEX_PERSON].num );
  SJGL_Blit( &(REC){0,246,20,10}, sc, sd, r-1 );
}

void obj_player_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  int i;
  int slot0;
  PLAYER_t *oldme = oa->data;
  PLAYER_t *newme = ob->data;
  GHOST_t *gh = fr[b].objs[newme->ghost].data;

  switch( fr[b].cmds[gh->client].cmd ) {
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
    case CMDT_1SEL:   newme->jumping = 1;                      break;
    case CMDT_0SEL:   newme->jumping = 0;                      break;
    case CMDT_1BACK:  newme->firing  = 1;                      break;
    case CMDT_0BACK:  newme->firing  = 0; newme->cooldown = 0; break;
  }

  if( !oldme ) { //FIXME why's this null?
    SJC_Write("Warning: oldme is NULL!");
    return;
  }

  gh->vel.x = newme->pos.x - gh->pos.x; //put ghost in the right spot
  gh->vel.z = newme->pos.z - gh->pos.z;

  if( ((GHOST_t *)fr[b].objs[newme->ghost].data)->client==me ) { //local client match
    v_camx = POINT2NATIVE_X(&gh->pos);
    v_camy = POINT2NATIVE_Y(&gh->pos);
    if( setmodel>-1 ) { //FIXME -- just for fun, will not sync!
      newme->model = setmodel;
      setmodel = -1;
    }
  }

  if( newme->firing || (newme->stabbing<0 && !newme->goingu)
                    || (newme->stabbing>0 && !newme->goingd) ) //firing or stopping pressing up/down
    newme->stabbing = 0;
  else if( !newme->stabbing && newme->vel.y!=0.0 ) {       //freefalling, not stabbing
    if( newme->goingu && !oldme->goingu )                  //just pressed up
      newme->stabbing = -4;
    if( newme->goingd && !oldme->goingd )                  //just pressed down
      newme->stabbing = 4;
  }
  if( newme->stabbing==-1 || newme->stabbing==1 )          //last frame of stabbing is over
    newme->stabbing = 0;
  if( newme->stabbing && newme->vel.y==0.0 ) {             //tink tink tink!
    if( newme->stabbing>0 )
      newme->vel.y -= 0.8f*oldme->vel.y;
    newme->stabbing += (newme->stabbing>0 ? -1 : 1);
  }
  if(        newme->stabbing && newme->goingu ) {          //expand hull for stabbing
    newme->hull[0].y = -29;
    newme->hull[1].y =  15;
  } else if( newme->stabbing && newme->goingd ) {
    newme->hull[0].y = -15;
    newme->hull[1].y =  25;
  } else {
    newme->hull[0].y = -15;
    newme->hull[1].y =  15;
  }

  newme->gunback = 0; //reset gun position
  if(newme->goingr||newme->goingl)
    newme->gundown = (newme->gundown+1)%10;
  else
    newme->gundown = 0;

  // friction
  #define P_FRIC(velxz,amt)                            \
    if(      newme->velxz> amt ) newme->velxz -= amt;  \
    else if( newme->velxz>-amt ) newme->velxz  = 0.0f; \
    else                         newme->velxz += amt;
  P_FRIC( vel.x,0.2f)
  P_FRIC(pvel.x,0.5f)
  P_FRIC( vel.z,0.2f)
  P_FRIC(pvel.z,0.5f)
  #undef P_FRIC

  // -- WALK --
  int keys;
  if( newme->turning )
    newme->turning--;
  #define P_MOVE(dir1, dir2, xz, plus, gt)                \
    if( (keys = newme->dir1 + newme->dir2) ) {            \
      newme->pvel.xz plus ## = (keys==1 ? 0.707f : 1.0f); \
      float max = 0 plus (keys==1 ? 2.121f : 3.0f);       \
      if( newme->pvel.xz gt max )                         \
        newme->pvel.xz = max;                             \
    }
  P_MOVE(goingl,goingu,x,-,<)
  P_MOVE(goingr,goingd,x,+,>)
  P_MOVE(goingu,goingr,z,-,<)
  P_MOVE(goingd,goingl,z,+,>)
  #undef P_MOVE

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
    newme->pvel.y  = -9.1f;          //initiate jump!

  // -- FIRE --
  if( newme->cooldown>0 )
    newme->cooldown--;
  if( newme->firing && newme->cooldown==0 && newme->projectiles<5 ) { // create bullet
    MKOBJ( bu, BULLET, ob->context, OBJF_POS|OBJF_VEL|OBJF_VIS );
    if( newme->facingr ) {
      bu->pos = (V){newme->pos.x+19,newme->pos.y-3,newme->pos.z};
      bu->vel = (V){ 6,0,-6};
    } else {
      bu->pos = (V){newme->pos.x-19,newme->pos.y-3,newme->pos.z};
      bu->vel = (V){-6,0, 6};
    }
    if( newme->goingu ) { // aiming
      bu->vel.y += -8;
      bu->pos.x += -2;
      bu->pos.y += -7;
    }
    if( newme->goingd ) {
      bu->vel.y +=  8;
      bu->pos.y += 16;
    }
    bu->model       = 1;
    bu->owner       = objid;
    bu->ttl         = 50;
    newme->cooldown = 5;
    newme->gunback  = 2;
    newme->projectiles++;
  }

  for(i=0;i<objid;i++)  //find other players to interact with -- who've already MOVED
    if(fr[b].objs[i].type==OBJT_PLAYER) {
      PLAYER_t *oldyou = fr[a].objs[i].data;
      PLAYER_t *newyou = fr[b].objs[i].data;
      if(    !oldyou
          || fabsf(newme->pos.x - newyou->pos.x)>5.0f //we're not on top of each other
          || fabsf(newme->pos.z - newyou->pos.z)>5.0f
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

  if( newme->hovertime ) { //gravity?
    newme->hovertime--;
    newme->vel.y += 0.3f;
  } else
    newme->vel.y += newme->hovertime ? 0.3f : 0.7f;
}

