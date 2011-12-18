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

void obj_slug_draw( int objid, OBJ_t *o )
{
  SLUG_t *sl = o->data;
  SJGL_SetTex( sys_tex[TEX_PLAYER].num );
  SJGL_Blit( &(SDL_Rect){(sl->vel.x>0?20:0)+(sl->dead?40:0),177,20,16}, sl->pos.x-10, sl->pos.y-8, sl->pos.y );
}

void obj_slug_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  int i;
  SLUG_t    *sl = ob->data;
  CONTEXT_t *co = fr[b].objs[ob->context].data;
  int kill = 0;
  sl->vel.y += 0.6f;      //gravity

  if( sl->dead )          //decay
    sl->dead++;
  else for(i=0;i<maxobjs;i++) { //find players, bullets to hit
    if(fr[b].objs[i].type==OBJT_BULLET) {
      BULLET_t *bu = fr[b].objs[i].data;
      if( fabsf(sl->pos.x - bu->pos.x)>8.0f || fabsf(sl->pos.y - bu->pos.y)>8.0f )
        continue; // no hit
      bu->ttl = 0;
      sl->vel.y = -3.0f;
      kill = 1;
    }
  }

  if( kill ) {
    sl->vel.x /= 100; //preserve direction while dead
    sl->dead = 1;
    ob->flags &= ~OBJF_PLAT;
  }

  if( sl->dead==5 )
    ob->flags &= ~(OBJF_CLIP|OBJF_BNDB);

  if(    sl->dead > 100 || sl->vel.x == 0 || sl->pos.x < -10.0f
      || sl->pos.x > co->x*co->blocksize+10.0f
      || sl->pos.y > co->y*co->blocksize+10.0f )
    ob->flags |= OBJF_DEL;
}

