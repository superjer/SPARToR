/**
 **  SPARToR
 **  Network Game Engine
 **  Copyright (C) 2010-2012  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/


#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"
#include "SDL_net.h"
#include "sjdl.h"
#include "main.h"
#include "sprite.h"
#include "sprite_helpers.h"
#include "console.h"
#include "helpers.h"
#include <limits.h>


SPRITE_T *sprite_grid_transform_xy(SPRITE_T *spr, CONTEXT_t *co, int x, int y, int z, int drawx, int drawy, int draww, int drawh)
{
  if( !spr->more )
    return spr;

  if( x<0 || y<0 || z<0 || x>=co->x || y>=co->y || z>=co->z )
    return spr;

  if( draww<2 && drawh<2 )
    return spr;

  CB *cb = co->dmap + z*co->y*co->z + y*co->x + x;
  int spr_num = (int)(spr - sprites);
  int samegrid = cb->spr >= spr_num && cb->spr <= spr->more->gridlast;
  const char *sprname = samegrid ? sprites[cb->spr].name : "";

  if( spr->more->piping ) {
    // always make connections to parts of the same drawing area, never remove existing connections
    int conn_u = drawy>0       || strstr(sprname,"12");
    int conn_r = drawx<draww-1 || strstr(sprname, "3");
    int conn_d = drawy<drawh-1 || strstr(sprname, "6");
    int conn_l = drawx>0       || strstr(sprname, "9");

    int i;
    for( i = spr_num; i <= spr->more->gridlast; i++ ) {
      sprname = sprites[i].name;
      if(    (strstr(sprname,"12") && 1) == conn_u
          && (strstr(sprname, "3") && 1) == conn_r
          && (strstr(sprname, "6") && 1) == conn_d
          && (strstr(sprname, "9") && 1) == conn_l )
        return sprites+i;
    }
  }

  if( spr->more->stretch ) {
    int stretch_t = spr->more->stretch_t;
    int stretch_r = spr->more->stretch_r;
    int stretch_b = spr->more->stretch_b;
    int stretch_l = spr->more->stretch_l;

    int gridwide = spr->more->gridwide;
    int gridtall = (spr->more->gridlast - spr_num + 1) / gridwide;

    // no sprites in the grid?
    if( gridwide<1 || gridtall<1 )
      return spr;

    // do NOT allow <= 0 sized middle section
    if( gridwide - stretch_l - stretch_r <= 0 ) stretch_l = stretch_r = 0;
    if( gridtall - stretch_t - stretch_b <= 0 ) stretch_t = stretch_b = 0;

    int t =       drawy  <stretch_t;
    int r = draww-drawx-1<stretch_r;
    int b = drawh-drawy-1<stretch_b;
    int l =       drawx  <stretch_l;

    int midw = l||r ? INT_MAX : MAX(1, gridwide - stretch_l - stretch_r);
    int midh = t||b ? INT_MAX : MAX(1, gridtall - stretch_t - stretch_b);

    int orig_x = l ? 0 : (r ? gridwide-stretch_r : stretch_l);
    int orig_y = t ? 0 : (b ? gridtall-stretch_b : stretch_t);

    // how far into the stretch region should we sample from?
    if( !l ) drawx -= stretch_l;
    if(  r ) drawx -= draww-stretch_r-1;
    if( !t ) drawy -= stretch_t;
    if(  b ) drawy -= drawh-stretch_b-1;

    drawx = (drawx % midw) + orig_x;
    drawy = (drawy % midh) + orig_y;

    return spr + drawx + gridwide*drawy;
  }

  return spr;
}

void renumber_sprites()
{
  if( !sprites || !old_sprites ) {
    SJC_Write("Can't renumber sprites!");
    return;
  }

  CB **blocks = NULL;

  // find and renumber ALL sprites, past and future
  int i, j, k;
  for( i=0; i<maxframes; i++ ) {
    for( j=0; j<maxobjs; j++ ) {
      if( fr[i].objs[j].type == OBJT_CONTEXT ) {
        CONTEXT_t *co = fr[i].objs[j].data;
        CB *map  = co->map;
        CB *dmap = co->dmap;
        int volume = co->x * co->y * co->z;

        if( pointlis_add( (void***)&blocks, map ) )
          for( k=0; k<volume; k++ )
            co->map[k].spr = find_sprite_by_name(old_sprites[co->map[k].spr].name);

        if( pointlis_add( (void***)&blocks, dmap ) )
          for( k=0; k<volume; k++ )
            co->dmap[k].spr = find_sprite_by_name(old_sprites[co->dmap[k].spr].name);
      }
    }
  }

  free(blocks);
}
