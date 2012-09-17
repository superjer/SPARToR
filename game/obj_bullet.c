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

void obj_bullet_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co )
{
  BULLET_t *bu = o->data;
  int g = bu->pos.x;
  int h = bu->pos.y;
  SJGL_SetTex( sys_tex[TEX_PLAYER].num );
  SJGL_Blit( &(REC){144,150,4,4}, g-2, h-2, NATIVEH );
}

void obj_bullet_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
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

  if(bu->pos.x<=-10.0f || bu->pos.x>=co->x*co->bsx+10.0f || bu->ttl==0) {
    if( fr[b].objs[bu->owner].type==OBJT_PLAYER )
      ((PLAYER_t *)fr[b].objs[bu->owner].data)->projectiles--;
    ob->flags |= OBJF_DEL;
  }
}

