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
#include "sprite.h"
#include "sprite_helpers.h"

#define SLUG_VEL 1.0f

void obj_slug_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co )
{
  SLUG_t *sl = o->data;
  int c = POINT2NATIVE_X(&sl->pos);
  int d = POINT2NATIVE_Y(&sl->pos);

  if( sl->dead )
    sprblit( sl->vel.x<0 ? &SM(slug_ouch_r) : &SM(slug_ouch_l), c, d, 0 );
  else 
    sprblit( sl->vel.x<0 ? &SM(slug_r)      : &SM(slug_l)     , c, d, 0 );
}

void obj_slug_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  int i;
  SLUG_t    *sl    = ob->data;
  SLUG_t    *oldsl = oa->data;
  CONTEXT_t *co    = fr[b].objs[ob->context].data;

  if( !sl || !oldsl )
    return;

  if( sl->dead )          //decay
    sl->dead++;
  else for(i=0;i<maxobjs;i++) { //find other slugs to hit
    if(fr[b].objs[i].type==OBJT_SLUG) {

      SLUG_t *you    = fr[b].objs[i].data;
      SLUG_t *oldyou = fr[a].objs[i].data;

      if( !you || !oldyou )
        continue;

      if( fabsf( sl->pos.x - you->pos.x)>30.1f || fabsf( sl->pos.y - you->pos.y)>32.1f )
        continue; // no hit

      float diffvelx    =    sl->vel.x -    you->vel.x;
      float diffvely    =    sl->vel.y -    you->vel.y;
      float olddiffvelx = oldsl->vel.x - oldyou->vel.x;
      float olddiffvely = oldsl->vel.y - oldyou->vel.y;
      if(    (   diffvelx*   diffvelx +    diffvely*   diffvely < 50.0f)
          && (olddiffvelx*olddiffvelx + olddiffvely*olddiffvely < 50.0f) )
        continue; // low velocity

      sl->vel.y = -6.0f;
      sl->vel.x = sl->pos.x > you->pos.x ? 6.0f : -6.0f;
      sl->dead = 1;
      ob->flags &= ~OBJF_PLAT;

      you->vel.y = -6.0f;
      you->vel.x = you->pos.x > sl->pos.x ? 6.0f : -6.0f;
      you->dead = 1;
      fr[b].objs[i].flags &= ~OBJF_PLAT;
    }
  }

  float fric = sl->vel.y==0.0f ? 0.5f : 0.0f;

  if( !fric ) { // air
    sl->vel.x *= 0.95f;
  } else { // ground
    if( sl->vel.x > 0.5f ) {
      if( sl->vel.x >  0.5f+fric ) sl->vel.x -=  fric;
      else                         sl->vel.x  =  0.5f;
    } else if( sl->vel.x < -0.5f ) {
      if( sl->vel.x < -0.5f-fric ) sl->vel.x +=  fric;
      else                         sl->vel.x  = -0.5f;
    }
  }

  // maintain minimum movement speed
  if( sl->vel.x > 0.0f && sl->vel.x <  SLUG_VEL ) sl->vel.x =  SLUG_VEL;
  if( sl->vel.x < 0.0f && sl->vel.x > -SLUG_VEL ) sl->vel.x = -SLUG_VEL;

  if( sl->dead==5 )
    ob->flags &= ~(OBJF_CLIP|OBJF_BNDB);

  if(    sl->dead > 100 || sl->vel.x == 0 || sl->pos.x < -10.0f
      || sl->pos.x > co->x*co->bsx+10.0f
      || sl->pos.y > co->y*co->bsy+10.0f )
    ob->flags |= OBJF_DEL;

  sl->vel.y += 1.0001f; //gravity
}

