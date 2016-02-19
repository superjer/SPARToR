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

#include "mod.h"
#include "audio.h"

PROTO_DRAW(SLUG)
{
  SLUG_t *sl = o->data;

  if( sl->vel.x>0 )
    sprblit( sl->dead ? &SM(slug_ouch_r) : &SM(slug_r), sl->pos.x, sl->pos.y );
  else
    sprblit( sl->dead ? &SM(slug_ouch_l) : &SM(slug_l), sl->pos.x, sl->pos.y );
}

PROTO_ADVANCE(SLUG)
{
  int i;
  SLUG_t    *sl = ob->data;
  CONTEXT_t *co = fr[b].objs[ob->context].data;
  int kill = 0;
  sl->vel.y += 0.6f;      //gravity

  if( sl->dead )          //decay
  {
    sl->dead++;
  }
  else for( i=0;i<maxobjs;i++ ) //find players, bullets to hit
  {
    if( fr[b].objs[i].type==OBJT_PLAYER )
    {
      PLAYER_t *pl = fr[b].objs[i].data;
      int up_stabbed = pl->stabbing<0
                    && fabsf(sl->pos.x                 - pl->pos.x                )<=14.0f
                    && fabsf(sl->pos.y + sl->hull[1].y - pl->pos.y - pl->hull[0].y)<=8.0f ;
      int dn_stabbed = pl->stabbing>0
                    && fabsf(sl->pos.x                 - pl->pos.x                )<=14.0f
                    && fabsf(sl->pos.y + sl->hull[0].y - pl->pos.y - pl->hull[1].y)<=4.0f ;
      if( up_stabbed )
      {
        pl->vel.y = sl->vel.y;
        sl->vel.y = -5.0f;
        kill = 1;
        play("stab");
      }
      else if( dn_stabbed )
      {
        pl->vel.y = -4.5f;
        pl->hovertime = 7;
        sl->vel.y = 0.0f;
        kill = 1;
        play("stab");
      }
    }
    else if( fr[b].objs[i].type==OBJT_BULLET )
    {
      BULLET_t *bu = fr[b].objs[i].data;
      if( fabsf(sl->pos.x - bu->pos.x)>8.0f || fabsf(sl->pos.y - bu->pos.y)>8.0f )
        continue; // no hit
      bu->ttl = 0;
      sl->vel.y = -3.0f;
      kill = 1;
      play("wibbity");
    }
  }

  if( kill )
  {
    sl->vel.x /= 100; //preserve direction while dead
    sl->dead = 1;
    ob->flags &= ~OBJF_PLAT;
  }

  if( sl->dead==5 )
    ob->flags &= ~(OBJF_CLIP|OBJF_BNDB);

  if(    sl->dead > 100 || sl->vel.x == 0 || sl->pos.x < -10.0f
      || sl->pos.x > co->x*co->bsx+10.0f
      || sl->pos.y > co->y*co->bsy+10.0f )
    ob->flags |= OBJF_DEL;
}

