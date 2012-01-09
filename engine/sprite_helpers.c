
#include <GL/glew.h>
#include "SDL.h"
#include "SDL_net.h"
#include "sjdl.h"
#include "main.h"
#include "sprite.h"


SPRITE_T *sprite_grid_transform_xy(SPRITE_T *spr, CONTEXT_t *co, int x, int y, int z, int drawx, int drawy, int draww, int drawh)
{
  if( !spr->more )
    return spr;

  if( x<0 || y<0 || z<0 || x>=co->x || y>=co->y || z>=co->z )
    return spr;

  CB *cb = co->dmap + z*co->y*co->z + y*co->x + x;
  int spr_num = (int)(spr - sprites);
  fprintf(stderr,"spr_num:%d  xyz:%d %d %d  draw:%d %d %d %d\n",spr_num,x,y,z,drawx,drawy,draww,drawh);
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
    //SJC_Write("STREEEEEEEEEEEEEEEETCH!");
  }

  return spr;
}


