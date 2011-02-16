/**
 **  mcdiddy's game
 **  implementation example for the spartor network game engine
 **  copyright (c) 2010-2011  jer wilson
 **
 **  see copying for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/spartor
 **/

#include "obj_.h"

void obj_dummy_draw( int objid, OBJ_t *o )
{
  DUMMY_t *du = o->data;
  SDL_Rect drect = (SDL_Rect){du->pos.x    +du->hull[0].x, du->pos.y    +du->hull[0].y,
                              du->hull[1].x-du->hull[0].x, du->hull[1].y-du->hull[0].y};
  Sint16 offs = drect.w==drect.h ? 48 : 0;

  if( drect.w > drect.h ) while( drect.w>0 && drect.w<400 ) {
    SJGL_BlitScaled(textures[TEX_WORLD], &(SDL_Rect){0+offs,16,16,16},
                                         &(SDL_Rect){drect.x,drect.y,drect.w,drect.h}, scale, 0);
    drect.x += 16;
    drect.w -= 16;
    offs = drect.w==16 ? 32 : 16;
  } else                  while( drect.h>0 && drect.h<400 ) {
    SJGL_BlitScaled(textures[TEX_WORLD], &(SDL_Rect){48,0+offs,16,16},
                                         &(SDL_Rect){drect.x,drect.y,drect.w,drect.h}, scale, 0);
    drect.y += 16;
    drect.h -= 16;
    offs = drect.h==16 ? 32 : 16;
  }
}

void obj_dummy_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  DUMMY_t *du = ob->data;

  // friction
  if(      du->vel.x> 0.1f ) du->vel.x -= 0.1f;
  else if( du->vel.x>-0.1f ) du->vel.x  = 0.0f;
  else                       du->vel.x += 0.1f;

  if( objid==(hotfr+100)%2000 ) //tee-hee
    du->vel.x += (float)(b%4)-1.5;

  du->vel.y += 0.7f;        //gravity
}

