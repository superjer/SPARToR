/**
 **  McDiddy's: The Game: Vengeance
 **  Implementation example for the SPARToR Network Game Engine
 **  Copyright (c) 2010-2015  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "obj_.h"
#include "sprite.h"
#include "audio.h"

//FIXME REMOVE! change local player model
int    setmodel = -1;
//


void obj_player_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co )
{
  PLAYER_t *pl = o->data;
  int c = pl->pos.x;
  int d = pl->pos.y;

  //girl hair
  if     ( pl->model!=4 ) ;
  else if( pl->facingr  ) sprblit( &SM(girlhair_r), c, d-30+(pl->goingd?4:0)+pl->gundown/7 );
  else                    sprblit( &SM(girlhair_l), c, d-30+(pl->goingd?4:0)+pl->gundown/7 );

  //player sprite
  if( pl->goingd )
  {
    if     ( pl->turning ) sprblit( &SM(ctblue_duck_f), c, d );
    else if( pl->facingr ) sprblit( &SM(ctblue_duck_r), c, d );
    else                   sprblit( &SM(ctblue_duck_l), c, d );
  }
  else
  {
    if     ( pl->turning ) sprblit( &SM(ctblue_f),      c, d );
    else if( pl->facingr ) sprblit( &SM(ctblue_r),      c, d );
    else                   sprblit( &SM(ctblue_l),      c, d );
  }

  // knife or gun
  if     ( pl->stabbing<0 ) sprblit( &SM(knife_up),   c, d-44 );
  else if( pl->stabbing>0 ) sprblit( &SM(knife_down), c, d+10 );
  else
  {
    int gb = pl->gunback;
    int gd = pl->gundown/5;

    if     ( pl->goingu && pl->facingr ) sprblit( &SM(mp5_up_r),   c+20-gb, d-25+gd );
    else if( pl->goingu                ) sprblit( &SM(mp5_up_l),   c-20+gb, d-25+gd );
    else if( pl->goingd && pl->facingr ) sprblit( &SM(mp5_down_r), c+20-gb, d-10+gd );
    else if( pl->goingd                ) sprblit( &SM(mp5_down_l), c-20+gb, d-10+gd );
    else if(               pl->facingr ) sprblit( &SM(mp5_r),      c+20-gb, d-19+gd );
    else                                 sprblit( &SM(mp5_l),      c-20+gb, d-19+gd );
  }
}

void obj_player_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  int i;
  int slot0;
  PLAYER_t *oldme = oa->data;
  PLAYER_t *newme = ob->data;
  GHOST_t *gh = fr[b].objs[newme->ghost].data;

  switch( fr[b].cmds[gh->client].cmd )
  {
    case CMDT_1LEFT:  newme->goingl  = 1;
                      if(  newme->facingr ) newme->turning = 3;
                      newme->facingr = 0;                      break;
    case CMDT_0LEFT:  newme->goingl  = 0;                      break;
    case CMDT_1RIGHT: newme->goingr  = 1;
                      if( !newme->facingr ) newme->turning = 3;
                      newme->facingr = 1;                      break;
    case CMDT_0RIGHT: newme->goingr  = 0;                      break;
    case CMDT_1UP:    newme->goingu  = 1;                      break;
    case CMDT_0UP:    newme->goingu  = 0;                      break;
    case CMDT_1DOWN:  newme->goingd  = 1;                      break;
    case CMDT_0DOWN:  newme->goingd  = 0;                      break;
    case CMDT_1JUMP:  newme->jumping = 1;                      break;
    case CMDT_0JUMP:  newme->jumping = 0;                      break;
    case CMDT_1FIRE:  newme->firing  = 1;                      break;
    case CMDT_0FIRE:  newme->firing  = 0; newme->cooldown = 0; break;
  }

  if( !oldme ) //FIXME why's this null?
  {
    echo("Warning: oldme is NULL!");
    return;
  }

  gh->vel.x = newme->pos.x - gh->pos.x; // get ghost heading in right direction
  gh->vel.y = newme->pos.y - gh->pos.y;
  gh->vel.z = newme->pos.z - gh->pos.z;

  if( gh->client==me ) //local client match
  {
    v_camx = gh->pos.x;
    v_camy = gh->pos.y;

    if( setmodel>-1 ) //FIXME -- just for fun, will not sync!
    {
      newme->model = setmodel;
      setmodel = -1;
    }
  }

  if( newme->firing || (newme->stabbing<0 && !newme->goingu)
                    || (newme->stabbing>0 && !newme->goingd) ) //firing or stopping pressing up/down
  {
    newme->stabbing = 0;
  }
  else if( !newme->stabbing && newme->vel.y!=0.0 )         //freefalling, not stabbing
  {
    if( newme->goingu && !oldme->goingu )                  //just pressed up
      newme->stabbing = -4;
    if( newme->goingd && !oldme->goingd )                  //just pressed down
      newme->stabbing = 4;
  }

  if( newme->stabbing==-1 || newme->stabbing==1 )          //last frame of stabbing is over
    newme->stabbing = 0;

  if( newme->stabbing && newme->vel.y==0.0 )               //tink tink tink!
  {
    if( newme->stabbing>0 )
    {
      newme->vel.y -= 0.8f*oldme->vel.y;
      play("tink");
    }
    else
    {
      play("tink2");
    }

    newme->stabbing += (newme->stabbing>0 ? -1 : 1);
  }

  if(        newme->stabbing && newme->goingu )          //expand hull for stabbing
  {
    newme->hull[0].y = -44;
    newme->hull[1].y =   0;
  }
  else if( newme->stabbing && newme->goingd )
  {
    newme->hull[0].y = -30;
    newme->hull[1].y =  10;
  }
  else
  {
    newme->hull[0].y = -30;
    newme->hull[1].y =   0;
  }

  newme->gunback = 0; //reset gun position

  if( newme->goingr || newme->goingl )
    newme->gundown = (newme->gundown+1)%10;
  else
    newme->gundown = 0;

  // friction
  #define P_FRIC(velxz,amt)                            \
    if(      newme->velxz> amt ) newme->velxz -= amt;  \
    else if( newme->velxz>-amt ) newme->velxz  = 0.0f; \
    else                         newme->velxz += amt;
  P_FRIC( vel.x, 0.2f)
  P_FRIC(pvel.x, 0.5f)
  #undef P_FRIC

  if( newme->turning )
    newme->turning--;

  // -- WALK --
  if( newme->goingl )
    newme->vel.x = newme->vel.x<-2.0f ? -2.0f : newme->vel.x-1.0f;
  if( newme->goingr )
    newme->vel.x = newme->vel.x> 2.0f ?  2.0f : newme->vel.x+1.0f;

  // -- JUMP --
  if( newme->pvel.y <= -2.0f )    //jumping in progress
  {
    newme->pvel.y += 2.0f;        //jumpvel fades into real velocity
    newme->vel.y += -2.0f;
  }
  else if( newme->pvel.y < 0.0f ) //jumping ending
  {
    newme->vel.y += newme->pvel.y;
    newme->pvel.y = 0.0f;
    newme->jumping = 0;           //must press jump again now
  }

  if( !newme->jumping )           //low-jump, cancel jump velocity early
    newme->pvel.y = 0.0f;

  if( (newme->vel.y==0.0f || oldme->vel.y==0.0f) && newme->jumping ) //FIXME 0 velocity means grounded? not really
  {
    newme->pvel.y = -9.1f; //initiate jump!
    play("jump2");
  }

  if( newme->vel.y==0.0f )
  {
    if( oldme->vel.y>0.0f && oldme->pos.y < newme->pos.y )
    {
      play("footstep1");
      newme->walkcounter = 0;
    }

    if( oldme->vel.y<0.0f )
      play("headbump");
  }

  if( newme->vel.y==0.0f && fabsf(newme->vel.x)>0.9f )
  {
    newme->walkcounter = (newme->walkcounter+1) % 14;
    if( newme->walkcounter==0 )
      play("footstep1");
    if( newme->walkcounter==7 )
      play("footstep2");
  }

  // -- FIRE --
  if( newme->cooldown>0 )
    newme->cooldown--;
  if( newme->firing && newme->cooldown==0 && newme->projectiles<5 ) // create bullet
  {
    MKOBJ( bu, BULLET, ob->context, OBJF_POS|OBJF_VEL|OBJF_VIS );
    play("mp5_shot");

    if( newme->facingr )
    {
      bu->pos = (V){newme->pos.x+19,newme->pos.y-19,newme->pos.z};
      bu->vel = (V){ 6,0,0};
    }
    else
    {
      bu->pos = (V){newme->pos.x-19,newme->pos.y-19,newme->pos.z};
      bu->vel = (V){-6,0,0};
    }

    if( newme->goingu ) // aiming
    {
      bu->vel.y += -8;
      bu->pos.y += -6;
    }

    if( newme->goingd )
    {
      bu->vel.y +=  8;
      bu->pos.y += 10;
    }

    bu->dead        = 0;
    bu->model       = 1;
    bu->owner       = objid;
    bu->ttl         = 50;
    newme->cooldown = 5;
    newme->gunback  = 2;
    newme->projectiles++;
  }

  for( i=0; i<objid; i++ )  //find other players to interact with -- who've already MOVED
    if( fr[b].objs[i].type==OBJT_PLAYER )
    {
      PLAYER_t *oldyou = fr[a].objs[i].data;
      PLAYER_t *newyou = fr[b].objs[i].data;

      if(    !oldyou
          || fabsf(newme->pos.x - newyou->pos.x)>5.0f //we're not on top of each other
          || fabsf(newme->pos.z - newyou->pos.z)>5.0f
          || fabsf(newme->pos.y - newyou->pos.y)>2.0f
          || newme->goingr  ||  newme->goingl         //or we're moving
          || newyou->goingr || newyou->goingl         )
        continue;

      if( newme->pos.x < newyou->pos.x )
      {
        newme->pvel.x  -= 0.2f;
        newyou->pvel.x += 0.2f;
      }
      else
      {
        newme->pvel.x  += 0.2f;
        newyou->pvel.x -= 0.2f;
      }
    }

  if( newme->hovertime ) //gravity?
  {
    newme->hovertime--;
    newme->vel.y += 0.1f;
  }
  else
  {
    newme->vel.y += 0.7f;
  }
}

