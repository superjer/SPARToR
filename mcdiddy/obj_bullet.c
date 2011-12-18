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

void obj_bullet_draw( int objid, OBJ_t *o )
{
  BULLET_t *bu = o->data;
  SJGL_SetTex( sys_tex[TEX_PLAYER].num );
  SJGL_Blit( &(SDL_Rect){144,150,4,4}, bu->pos.x-2, bu->pos.y-2, NATIVEH );
}

void obj_bullet_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  /*
  int i;
  BULLET_t  *bu = ob->data;
  CONTEXT_t *co = fr[b].objs[ob->context].data;

  if( bu->ttl ) bu->ttl--;

  for(i=0;i<maxobjs;i++)  //find players to hit
    if(fr[b].objs[i].type==OBJT_PLAYER) {
      PLAYER_t *pl = fr[b].objs[i].data;
      if( i==bu->owner                       || //player owns bullet
          fabsf(bu->pos.x - pl->pos.x)>10.0f || //not touching
          fabsf(bu->pos.y - pl->pos.y)>15.0f    )
        continue;
      pl->vel.y += -5.0f;
      pl->vel.x += (bu->vel.x>0.0f?5.0f:-5.0f);
      bu->ttl = 0; //delete bullet
    }

  if(bu->pos.x<=-10.0f || bu->pos.x>=co->x*co->blocksize+10.0f || bu->ttl==0) {
    if( fr[b].objs[bu->owner].type==OBJT_PLAYER )
      ((PLAYER_t *)fr[b].objs[bu->owner].data)->projectiles--;
    ob->flags |= OBJF_DEL;
  }
  */
}

