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
#include "saveload.h"
#include "sprite.h"
#include "sprite_helpers.h"


static void ghost_paint( FCMD_t *c, GHOST_t *gh, PLAYER_t *pl, CONTEXT_t *co );


void obj_ghost_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co )
{
  GHOST_t *gh = o->data;

  if( !v_drawhulls ) return;

  int g = TILE2NATIVE_X(co, gh->pos.x, gh->pos.y, gh->pos.z);
  int h = TILE2NATIVE_Y(co, gh->pos.x, gh->pos.y, gh->pos.z);

  SJGL_SetTex( sys_tex[TEX_PLAYER].num );
  SJGL_Blit( &(REC){80,177,16,16}, g, h, NATIVEH );
}

void obj_ghost_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  GHOST_t   *gh = ob->data;
  PLAYER_t  *pl = fr[b].objs[gh->avatar].data;
  CONTEXT_t *co = fr[b].objs[ob->context].data;

  if( gh->client==me ) {
    myghost     = objid;
    mycontext   = ob->context;
  }

  static V v2_dimetric[2] = {{-64,-72,-64},{64,  0, 64}};
  static V v2_ortho[2]    = {{-NATIVEW/2,-NATIVEH/2,0},{NATIVEW/2,NATIVEH/2,0}};

  if( co->projection == DIMETRIC     )
    memcpy( gh->hull, v2_dimetric, sizeof (V[2]) );
  if( co->projection == ORTHOGRAPHIC )
    memcpy( gh->hull, v2_ortho,    sizeof (V[2]) );

  FCMD_t *c = fr[b].cmds + gh->client;

  switch( c->cmd ) {
  case CMDT_0CON: { //FIXME: edit rights!
    size_t n = 0;
    char letter = (char)unpackbytes(c->data,MAXCMDDATA,&n,1);

    switch( letter ) {
    case 'o': // orthographic
      co->projection = ORTHOGRAPHIC;
      SJC_Write("Setting context projection to ORTHOGRAPHIC");
      break;

    case 'd': // dimetric
      co->projection = DIMETRIC;
      SJC_Write("Setting context projection to DIMETRIC");
      break;

    case 'b': { // bounds
      int x = (int)unpackbytes(c->data,MAXCMDDATA,&n,4);
      int y = (int)unpackbytes(c->data,MAXCMDDATA,&n,4);
      int z = (int)unpackbytes(c->data,MAXCMDDATA,&n,4);

      CONTEXT_t tmp;
      const char *error = create_context(&tmp, co, x, y, z);

      if( error )
        SJC_Write("%s", error);
      else
        memcpy(co, &tmp, sizeof tmp);

      break; }

    case 'z': { // blocksize
      co->bsx = (int)unpackbytes(c->data,MAXCMDDATA,&n,4);
      co->bsy = (int)unpackbytes(c->data,MAXCMDDATA,&n,4);
      co->bsz = (int)unpackbytes(c->data,MAXCMDDATA,&n,4);

      break; }

    case 't': { // tilespacing
      co->tileuw = (int)unpackbytes(c->data,MAXCMDDATA,&n,4);
      co->tileuh = (int)unpackbytes(c->data,MAXCMDDATA,&n,4);

      break; }

    default:
      SJC_Write("Unknown edit command!");
      break;
    }

    // do NOT free co->map, co->dmap, it will get GC'd as it rolls off the buffer! really!
    break; }

  case CMDT_0EPANT: //FIXME: UNSAFE check for edit rights, data values
    ghost_paint( c, gh, pl, co );
    break;
  }
}

static void ghost_paint( FCMD_t *c, GHOST_t *gh, PLAYER_t *pl, CONTEXT_t *co )
{
  size_t n = 0;
  int  i, j, k;
  char letter = (char)unpackbytes(c->data,MAXCMDDATA,&n,1);
  int  dnx    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
  int  dny    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
  int  dnz    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
  int  upx    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
  int  upy    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
  int  upz    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
  int  sprnum = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);

  if( letter!='p' ) { SJC_Write("Unknown edit command!"); return; }

  int tool_num = (sprites[sprnum].flags & TOOL_MASK);

  int shx = 0;
  int shy = 0;
  int shz = 0;

  int clipx = MAX(gh->clipboard_x,1);
  int clipy = MAX(gh->clipboard_y,1);
  int clipz = MAX(gh->clipboard_z,1);

  //make so dn is less than up... also adjust clipboard shift
  if( dnx > upx )  { SWAP(upx,dnx,int); shx = clipx-(upx-dnx+1)%clipx; }
  if( dny > upy )  { SWAP(upy,dny,int); shy = clipy-(upy-dny+1)%clipy; }
  if( dnz > upz )  { SWAP(upz,dnz,int); shz = clipz-(upz-dnz+1)%clipz; }

  if( dnx<0 || dny<0 || dnz<0 || upx>=co->x || upy>=co->y || upz>=co->z ) {
    SJC_Write("Paint command out of bounds!");
    return;
  }

  if( tool_num == TOOL_COPY ) { //COPY tool texture
    gh->clipboard_x = clipx = upx - dnx + 1;
    gh->clipboard_y = clipy = upy - dny + 1;
    gh->clipboard_z = clipz = upz - dnz + 1;
    gh->clipboard_data = malloc( clipx*clipy*clipz*(sizeof *gh->clipboard_data) ); //FIXME: mem leak
  }

  if( tool_num == TOOL_PSTE && !gh->clipboard_data ) { SJC_Write("Clipboard is empty"); return; }

  for( k=dnz; k<=upz; k++ ) for( j=dny; j<=upy; j++ ) for( i=dnx; i<=upx; i++ ) {
    int pos = k*co->y*co->x + j*co->x + i;

    if( !tool_num ) { // regular tile painting
      int dsprnum = sprnum;

      if( co->projection == ORTHOGRAPHIC )
        dsprnum = sprite_grid_transform_xy(sprites + sprnum, co, i, j, k, i-dnx, j-dny, upx-dnx+1, upy-dny+1) - sprites;

      co->dmap[pos].flags &= ~CBF_NULL;
      co->dmap[pos].flags |= CBF_VIS;
      co->dmap[pos].spr    = dsprnum;
      continue;
    }

    switch( tool_num ) {
    case TOOL_NUL:
      co->dmap[pos] = co->map[pos];
      co->dmap[pos].flags |= CBF_NULL;
      break;

    case TOOL_SOL:
      co->dmap[pos].flags &= ~(CBF_NULL|CBF_PLAT);
      co->dmap[pos].flags |= CBF_SOLID;
      break;

    case TOOL_PLAT:
      co->dmap[pos].flags &= ~(CBF_NULL|CBF_SOLID);
      co->dmap[pos].flags |= CBF_PLAT;
      break;

    case TOOL_OPN:
      co->dmap[pos].flags &= ~(CBF_NULL|CBF_SOLID|CBF_PLAT);
      break;

    case TOOL_COPY:
      gh->clipboard_data[ (k-dnz)*clipy*clipx + (j-dny)*clipx + (i-dnx) ] = co->dmap[pos];
      break;

    case TOOL_PSTE: {
      int x = (i-dnx+shx) % clipx;
      int y = (j-dny+shy) % clipy;
      int z = (k-dnz+shz) % clipz;
      co->dmap[pos] =gh->clipboard_data[ x + y*clipx + z*clipy*clipx ];
      break; }

    case TOOL_OBJ:
      pl->pos.x = i*co->bsx;
      pl->pos.y = j*co->bsy;
      pl->pos.z = k*co->bsz;
      break;

    case TOOL_ERAS:
      co->dmap[pos].flags &= ~(CBF_VIS|CBF_NULL);
      break;

    case TOOL_VIS:
      co->map[pos].flags |= CBF_VIS; // hack for making a loaded-from-file tile visible (format change mess)
      break;

    }
  }
}

