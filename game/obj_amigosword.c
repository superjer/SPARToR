/**
 **  McDiddy's: The Game: Vengeance
 **  Implementation example for the SPARToR Network Game Engine
 **  Copyright (c) 2010-2012  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "obj_.h"

void obj_amigosword_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co )
{
  AMIGOSWORD_t *sw = o->data;
  int posx = TILE2NATIVE_X(co,sw->pos.x/16,0,sw->pos.y/16);
  int posy = TILE2NATIVE_Y(co,sw->pos.x/16,0,sw->pos.y/16);
  SJGL_SetTex( sys_tex[TEX_AMIGO].num );
  SJGL_Blit( &(REC){ 200, 50+50*(hotfr%3), 56, 50 }, posx-25, posy-28, sw->pos.y );
}

void obj_amigosword_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  AMIGOSWORD_t *sw = ob->data;
  sw->spincounter++;
  if( sw->spincounter > 45 ) {
    AMIGO_t *am = fr[b].objs[sw->owner].data;
    sw->vel.x = am->pos.x+40.0f - sw->pos.x;
    sw->vel.y = am->pos.y       - sw->pos.y;
    float normalize = sqrt(sw->vel.x * sw->vel.x + sw->vel.y * sw->vel.y);
    if( normalize > 4.0f ) {
      normalize = 4.0f / normalize;
      sw->vel.x *= normalize;
      sw->vel.y *= normalize;
    }
  }
}

