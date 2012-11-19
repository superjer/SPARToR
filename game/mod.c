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

#include "mod.h"
#include "saveload.h"
#include "sjglob.h"
#include "sprite.h"
#include "sprite_helpers.h"
#include "video_helpers.h"
#include "command.h"
#include "keynames.h"
#include "helpers.h"
#include "gui.h"

SYS_TEX_T sys_tex[] = {{"/tool.png"       ,0},
                       {"/player.png"     ,0},
                       {"/persons.png"    ,0},
                       {"/blankhud.png"   ,0}};
size_t num_sys_tex = COUNTOF(sys_tex);

INPUTNAME_t inputnames[] = {{"left"       ,CMDT_1LEFT    ,CMDT_0LEFT    },
                            {"right"      ,CMDT_1RIGHT   ,CMDT_0RIGHT   },
                            {"up"         ,CMDT_1UP      ,CMDT_0UP      },
                            {"down"       ,CMDT_1DOWN    ,CMDT_0DOWN    },
                            {"nw"         ,CMDT_1NW      ,CMDT_0NW      },
                            {"ne"         ,CMDT_1NE      ,CMDT_0NE      },
                            {"sw"         ,CMDT_1SW      ,CMDT_0SW      },
                            {"se"         ,CMDT_1SE      ,CMDT_0SE      },
                            {"select"     ,CMDT_1SEL     ,CMDT_0SEL     },
                            {"back"       ,CMDT_1BACK    ,CMDT_0BACK    },
                            {"camleft"    ,CMDT_1CAMLEFT ,CMDT_0CAMLEFT },
                            {"camright"   ,CMDT_1CAMRIGHT,CMDT_0CAMRIGHT},
                            {"camup"      ,CMDT_1CAMUP   ,CMDT_0CAMUP   },
                            {"camdown"    ,CMDT_1CAMDOWN ,CMDT_0CAMDOWN },
                         /* {"cons-cmd"   ,CMDT_1CON     ,CMDT_0CON     }, this may not be necessary, it may even be dangerous */
                            {"edit-paint" ,CMDT_1EPANT   ,CMDT_0EPANT   },
                            {"edit-prev"  ,CMDT_1EPREV   ,CMDT_0EPREV   },
                            {"edit-next"  ,CMDT_1ENEXT   ,CMDT_0ENEXT   },
                            {"edit-texup" ,CMDT_1EPGUP   ,CMDT_0EPGUP   },
                            {"edit-texdn" ,CMDT_1EPGDN   ,CMDT_0EPGDN   },
                            {"edit-layup" ,CMDT_1ELAUP   ,CMDT_0ELAUP   },
                            {"edit-laydn" ,CMDT_1ELADN   ,CMDT_0ELADN   },
                            {"edit-show"  ,CMDT_1ESHOW   ,CMDT_0ESHOW   },
                            {"edit-undo"  ,CMDT_1EUNDO   ,CMDT_0EUNDO   }};
int numinputnames = COUNTOF(inputnames);

int    myghost;     //obj number of local player ghost
int    mycontext;

int    downx = -1; //position of mousedown at beginning of edit cmd
int    downy = -1;
int    downz = -1;
int    ylayer = 0; // tile layer
int    showlayer = 0;

int    setmodel; //FIXME REMOVE! change local player model
CB    *hack_map; //FIXME remove hack_map and _dmap someday
CB    *hack_dmap;

static int    binds_size = 0;
static struct {
  unsigned short sym;
  unsigned char  device;
  unsigned char  press;
  unsigned char  cmd;
  char          *script;
}            *binds;
static int    editmode = 0;
static int    myspr    = 0;
static int    mytex    = 0;
static FCMD_t magic_c;      // magical storage for an extra command, triggered from console

// prototypes
static int proc_edit_cmd(FCMD_t *c,int device,int sym,int press);
static int gui_click( int press );
static void screen_unproject( int screenx, int screeny, int height, int *x, int *y, int *z );
static void draw_sprite_on_tile( SPRITE_T *spr, CONTEXT_t *co, int x, int y, int z );
static int sprite_at(int texnum, int x, int y);

void mod_setup(Uint32 setupfr)
{
  //default key bindings
  exec_commands("defaults");

  //make the mother object
  fr[setupfr].objs[0] = (OBJ_t){ OBJT_MOTHER, 0, 0, sizeof(MOTHER_t), malloc(sizeof(MOTHER_t)) };
  memset( fr[setupfr].objs[0].data, 0, sizeof(MOTHER_t) );

  //make default context object (map)
  fr[setupfr].objs[1] = (OBJ_t){ OBJT_CONTEXT, OBJF_REFC, 0, sizeof(CONTEXT_t), malloc(sizeof(CONTEXT_t)) };
  CONTEXT_t *co = fr[setupfr].objs[1].data;
  co->bsx = co->bsy = co->bsz = 16;
  co->x   = co->y   = co->z   = 15;
  co->tileuw = 48;
  co->tileuh = 24;
  co->projection = DIMETRIC;
  int volume = co->x * co->y * co->z;
  co->map  = hack_map  = malloc( (sizeof *co->map ) * volume ); //FIXME remove hack
  co->dmap = hack_dmap = malloc( (sizeof *co->dmap) * volume );
  memset( co->map,  0, (sizeof *co->map ) * volume );
  memset( co->dmap, 0, (sizeof *co->dmap) * volume );
  int i;
  for( i=0; i<volume; i++ ) {
    co->map[ i].spr   = 0;
    co->dmap[i].flags = CBF_NULL;
  }
  load_context("dirtfarm",1,setupfr); //load a default map

  fr[setupfr+1].cmds[0].flags |= CMDF_NEW; //server is a client

  SJC_Write("Default controls: \\#F80A, S, Numpad Arrows, F11");
}

void mod_recvobj(OBJ_t *o)
{
  CONTEXT_t *co;

  switch( o->type ) {
    case OBJT_CONTEXT:
      co = o->data;
      co->map  = hack_map; // FIXME: horrible hack FOR NOW!
      co->dmap = hack_dmap;//        when receiving map, use data we already have
      SJC_Write("mod_recvobj(): reusing map data before network connect (hack)");
      break;
  }
}

void mod_setvideo(int w,int h)
{
  mod_loadsurfs(0);
}

void mod_quit()
{
  mod_loadsurfs(1);
}

void mod_showbinds()
{
  int i,j;

  for(i=0; i<binds_size; i++) {
    if( !binds[i].cmd && !binds[i].script )
      continue;

    char plusminus[3] = "";
    plusminus[0] = binds[i].press ? '+' : '-';

    const char *devname = "?";
    int device = MIN( INP_MAX, binds[i].device );
    if( binds[i].device==INP_KEYB )
      devname = "";
    else
      devname = inputdevicenames[device];

    char kbuf[10] = "badkey";
    const char *symname = kbuf;
    int sym = binds[i].sym;
    if( device==INP_KEYB && sym<KEYNAMECOUNT && keynames[sym] )
      symname = keynames[sym];
    else
      sprintf(kbuf,"%d",sym);

    char cbuf[10] = "badcmd";
    int cmd = binds[i].cmd;
    const char *cmdname = cbuf;
    for(j=0; j<numinputnames && cmd; j++)
      if( inputnames[j].presscmd==cmd || inputnames[j].releasecmd==cmd )
        cmdname = inputnames[j].name;

    if( binds[i].script ) {
      plusminus[1] = '!';
      cmdname = binds[i].script;
    }
    if( !cmdname )
      sprintf(cbuf,"%d",cmd);

    SJC_Write("bind %s%s %s%s",devname,symname,plusminus,cmdname);
  }
}

void mod_keybind(int device,int sym,int press,char cmd,char *script)
{
  int i;

  for(i=0; i<binds_size; i++)
    if( binds[i].device==0 ||
        (binds[i].sym==sym && binds[i].device==device && binds[i].press==press) )
      break;
  if(i==binds_size) {
    binds = realloc(binds,sizeof(*binds)*(binds_size+32));
    memset(binds+binds_size,0,sizeof(*binds)*32);
    binds_size += 32;
  }
  binds[i].sym = sym;
  binds[i].device = device;
  binds[i].press = press;
  binds[i].cmd = cmd;
  safe_free(binds[i].script);
  safe_copy(binds[i].script,script);
}

// returns 0 iff a command is created to be put on the network
int mod_mkcmd(FCMD_t *c,int device,int sym,int press)
{
  int i;

  // apply magic command?
  if( device==-1 ) {
    if( !magic_c.datasz )
      return -1;
    memcpy(c, &magic_c, sizeof magic_c);
    memset(&magic_c, 0, sizeof magic_c);
    return 0;
  }

  for(i=0; i<binds_size; i++) {
    if( binds[i].sym!=sym || binds[i].device!=device || binds[i].press!=press )
      continue;

    memset( c, 0, sizeof *c );

    if( binds[i].script ) {
      command( binds[i].script );
      return -1;
    }

    c->cmd = binds[i].cmd;

    if( c->cmd==CMDT_1EPANT || c->cmd==CMDT_0EPANT )
      if( gui_click( press ) )
        return -1;

    if( editmode )
      return proc_edit_cmd(c,device,sym,press);

    return c->cmd<=CMDT_1CON ? 0 : -1;
  }

  return -1;
}

// returns 0 iff the GUI did not eat the click
static int gui_click( int press )
{
  int elem = gui_element_at(hotfr,i_mousex,i_mousey);
  if( !elem || !press )
    return 0;

  POPUP_t *pop = fr[hotfr%maxframes].objs[elem].data;
  SJC_Write("Clicked on button: %s",pop->text);
  return 1;
}

static int proc_edit_cmd(FCMD_t *c,int device,int sym,int press)
{
  CONTEXT_t *co = fr[hotfr%maxframes].objs[mycontext].data;

  if( c->cmd==CMDT_0EPREV || c->cmd==CMDT_0ENEXT ) //these shouldn't really happen and wouldn't mean anything
    return -1;

  if( c->cmd==CMDT_1EPREV ) { //select previous tile
    if( !spr_count ) return -1;
    myspr = (myspr + spr_count - 1) % spr_count;
    mytex = sprites[myspr].texnum;
    return -1;
  }

  if( c->cmd==CMDT_1ENEXT ) { //select next tile
    if( !spr_count ) return -1;
    myspr = (myspr + 1) % spr_count;
    mytex = sprites[myspr].texnum;
    return -1;
  }

  if( c->cmd==CMDT_0EPGUP || c->cmd==CMDT_0EPGDN )
    return -1;

  if( c->cmd==CMDT_1EPGUP ) { //prev texture file
    if( textures[mytex].filename ) do {
      mytex = (mytex + tex_count - 1) % tex_count;
    } while( !textures[mytex].filename );
    return -1;
  }

  if( c->cmd==CMDT_1EPGDN ) { //next texture file
    if( textures[mytex].filename ) do {
      mytex = (mytex + 1) % tex_count;
    } while( !textures[mytex].filename );
    return -1;
  }

  if( c->cmd==CMDT_0ELAUP || c->cmd==CMDT_0ELADN )
    return -1;

  if( c->cmd==CMDT_1ELAUP )
    if( ylayer > 0 ) ylayer--;

  if( c->cmd==CMDT_1ELADN )
    if( ylayer < co->y ) ylayer++;

  if( c->cmd==CMDT_1ESHOW ) { showlayer = 1; return -1; }
  if( c->cmd==CMDT_0ESHOW ) { showlayer = 0; return -1; }

  if( c->cmd!=CMDT_1EPANT && c->cmd!=CMDT_0EPANT )
    return 0; //cmd created

  //edit-paint command only from here on
  int dnx = downx;
  int dny = downy;
  int dnz = downz;

  if( c->cmd==CMDT_0EPANT ) //always clear mousedown pos on mouseup
    downx = downy = downz = -1;

  if( !i_hasmouse )
    return -1;

  if( i_mousex >= v_w-NATIVE_TEX_SZ && i_mousey < NATIVE_TEX_SZ ) { //click in texture selector
    if( c->cmd==CMDT_1EPANT ) {
      int tmp = sprite_at(mytex, i_mousex-(v_w-NATIVE_TEX_SZ), i_mousey);
      if( tmp>=0 ) myspr = tmp;
    }
    return -1;
  }

  //map to game coordinates
  int tilex,tiley,tilez;
  screen_unproject( i_mousex, i_mousey, ylayer * co->bsy, &tilex, &tiley, &tilez );

  if( co->projection == DIMETRIC     ) tiley = ylayer;
  if( co->projection == ORTHOGRAPHIC ) tilez = ylayer;

  if( c->cmd==CMDT_1EPANT ) {
    downx = tilex;
    downy = tiley;
    downz = tilez;
    return -1; //finish cmd later...
  }

  if( dnx<0 || dny<0 || dnz<0 )
    return -1; //no mousedown? no cmd

  size_t n = 0;
  packbytes(c->data,  'p',&n,1);
  packbytes(c->data,  dnx,&n,4);
  packbytes(c->data,  dny,&n,4);
  packbytes(c->data,  dnz,&n,4);
  packbytes(c->data,tilex,&n,4);
  packbytes(c->data,tiley,&n,4);
  packbytes(c->data,tilez,&n,4);
  packbytes(c->data,myspr,&n,4);
  c->datasz = n;
  c->flags |= CMDF_DATA; //indicate presence of extra cmd data

  return 0;
}

int safe_atoi(const char *s)
{
  if( !s ) return 0;
  return atoi(s);
}

int mod_command(char *q,char *args)
{
  if( q==NULL ){
    ;

  }else if( strcmp(q,"edit")==0 ){
    editmode = editmode ? 0 : 1;
    v_center = editmode ? 0 : 1;
    return 0;

  }else if( strcmp(q,"model")==0 ){
    setmodel = safe_atoi(tok(args," ")); // FIXME: lame hack
    return 0;

  }else if( strcmp(q,"bounds")==0 || strcmp(q,"blocksize")==0 ){
    size_t n = 0;
    int x = safe_atoi(tok(args," "));
    int y = safe_atoi(tok(args," "));
    int z = safe_atoi(tok(args," "));
    char chr = strcmp(q,"bounds")==0 ? 'b' : 'z';

    if( !x || !y || !z ) {
      CONTEXT_t *co = fr[hotfr%maxframes].objs[mycontext].data; // FIXME is mycontext always set here?
      if( chr == 'b' )
        SJC_Write("The current bounds are (X,Y,Z): %d %d %d", co->x, co->y, co->z);
      else
        SJC_Write("The current blocksize is (X,Y,Z): %d %d %d", co->bsx, co->bsy, co->bsz);
      return 0;
    }

    memset(&magic_c,0,sizeof magic_c);
    packbytes(magic_c.data,chr,&n,1);
    packbytes(magic_c.data,  x,&n,4);
    packbytes(magic_c.data,  y,&n,4);
    packbytes(magic_c.data,  z,&n,4);
    magic_c.datasz = n;
    magic_c.flags |= CMDF_DATA;
    magic_c.cmd = CMDT_0CON; // console command
    putcmd(-1,-1,-1);
    return 0;

  }else if( strcmp(q,"tilespacing")==0 ){
    size_t n = 0;
    int tileuw = safe_atoi(tok(args," "));
    int tileuh = safe_atoi(tok(args," "));

    if( !tileuw || !tileuh ) {
      CONTEXT_t *co = fr[hotfr%maxframes].objs[mycontext].data;
      SJC_Write("The current tilespacing is (W,H): %d %d", co->tileuw, co->tileuh);
      return 0;
    }

    memset(&magic_c,0,sizeof magic_c);
    packbytes(magic_c.data,   't',&n,1);
    packbytes(magic_c.data,tileuw,&n,4);
    packbytes(magic_c.data,tileuh,&n,4);
    magic_c.datasz = n;
    magic_c.flags |= CMDF_DATA;
    magic_c.cmd = CMDT_0CON;
    putcmd(-1,-1,-1);
    return 0;

  }else if( strcmp(q,"orthographic")==0 || strcmp(q,"dimetric")==0 ){
    size_t n = 0;

    memset(&magic_c,0,sizeof magic_c);
    packbytes(magic_c.data,q[0],&n,1);
    magic_c.datasz = n;
    magic_c.flags |= CMDF_DATA;
    magic_c.cmd = CMDT_0CON;
    putcmd(-1,-1,-1);
    return 0;

  }else if( strcmp(q,"resprite")==0 ){
    reload_sprites();
    renumber_sprites();
    SJC_Write("Was %d sprites, now %d sprites.",old_spr_count,spr_count);
    unload_sprites(old_sprites,old_spr_count);
    return 0;
  }

  return 1;
}

void mod_loadsurfs(int quit)
{
  size_t i;

  // free existing textures
  for( i=0; i<tex_count; i++ )
    if( textures[i].generated ) {
      glDeleteTextures(1,&textures[i].glname);
      textures[i].generated = 0;
    }

  if( quit ) return;

  SJGLOB_T *files = SJglob( "game/textures", "*.png", SJGLOB_MARK|SJGLOB_NOESCAPE );

  for( i=0; i<files->gl_pathc; i++ )
    make_sure_texture_is_loaded( files->gl_pathv[i] );

  SJglobfree( files );
}

void mod_predraw(Uint32 vidfr)
{
  int i,j,k;

  glClear(GL_COLOR_BUFFER_BIT);

  //draw context
  CONTEXT_t *co = fr[vidfr%maxframes].objs[mycontext].data; // FIXME: is mycontext always set here?

  for( k=0; k<co->z; k++ ) for( j=0; j<co->y; j++ ) for( i=0; i<co->x; i++ ) {
    int pos = co->x*co->y*k + co->x*j + i;

    if( showlayer && ylayer!=j )
      continue;

    CB *cb = co->dmap + pos;
    if( !(cb->flags & CBF_VIS) )
      continue;

    SPRITE_T *spr = sprites + cb->spr;
    draw_sprite_on_tile( spr, co, i, j, k );
  }
}

void mod_draw(int objid,Uint32 vidfrmod,OBJ_t *o)
{
  if( !fr[vidfrmod].objs[o->context].type ) {
    SJC_Write("No context: can not draw object %d",objid);
    return;
  }

  CONTEXT_t *co = fr[vidfrmod].objs[o->context].data;
  switch(o->type) {
    case OBJT_GHOST:          obj_ghost_draw(      objid, vidfrmod, o, co );     break;
    case OBJT_PERSON:         obj_person_draw(     objid, vidfrmod, o, co );     break;
  }
}

void mod_huddraw(Uint32 vidfr)
{
  Uint32 vidfrmod = vidfr%maxframes;
  MOTHER_t *mo = fr[vidfrmod].objs[0].data;

  if( mo->active && mo->pc )
  {
    int x = 57;

    PERSON_t *pe = fr[vidfrmod%maxframes].objs[mo->active].data;

    SJGL_SetTex( sys_tex[TEX_HUD].num );
    SJGL_Blit( &(REC){0,0,160,50},   0, NATIVEH-50, 0 );

    #define BAR_W(stat) (pe->stat>0 ? 15+32*pe->stat/pe->max_##stat : 0)
    SJGL_Blit( &(REC){0,50+6*0,BAR_W(hp),6}, x   , NATIVEH-50+13+9*0, 0 );
    SJGL_Blit( &(REC){0,50+6*1,BAR_W(mp),6}, x+51, NATIVEH-50+13+9*0, 0 );
    SJGL_Blit( &(REC){0,50+6*2,BAR_W(st),6}, x   , NATIVEH-50+13+9*1, 0 );
    SJGL_Blit( &(REC){0,50+6*3,BAR_W(ap),6}, x+51, NATIVEH-50+13+9*1, 0 );
    SJGL_Blit( &(REC){0,50+6*4,BAR_W(pn),6}, x   , NATIVEH-50+13+9*2, 0 );
    SJGL_Blit( &(REC){0,50+6*5,BAR_W(ml),6}, x+51, NATIVEH-50+13+9*2, 0 );
    SJGL_Blit( &(REC){0,50+6*6,BAR_W(to),6}, x   , NATIVEH-50+13+9*3, 0 );
    SJGL_Blit( &(REC){0,50+6*7,BAR_W(xp),6}, x+51, NATIVEH-50+13+9*3, 0 );
    #undef BAR_W

    SJF_DrawText( 3, NATIVEH-49, SJF_LEFT, "%s", pe->name );
  }

  // draw menu background and then menu items
  SJGL_SetTex( 0 );
  SJGL_SetTex( sys_tex[TEX_HUD].num );
  SJGL_Blit( &(REC){512-75,0,75,150}, NATIVEW-75, 0, 0 );

  int i;
  for( i=0; i<maxobjs; i++ )
  {
    OBJ_t *ob = fr[vidfrmod].objs+i;
    if( ob->type != OBJT_POPUP ) continue;
    POPUP_t *pop = ob->data;
    V *pos  = flex(ob,pos);
    V *hull = flex(ob,hull);
    if( i==gui_hover ) {
      SJGL_SetTex( 0 );
      SJGL_SetTex( 2 ); // FIXME: NO NO NO!
      SJGL_Blit( &(REC){30,30,hull[1].x,hull[1].y}, pos->x, pos->y, 0 );
    }
    SJF_DrawText( pos->x, pos->y, SJF_LEFT, "%s", pop->text );
  }
}

void mod_postdraw(Uint32 vidfr)
{
  int i,j,k;

  if( !editmode || !i_hasmouse ) return;

  GHOST_t   *gh = fr[vidfr%maxframes].objs[myghost].data; // FIXME is myghost/mycontext always set here?
  CONTEXT_t *co = fr[vidfr%maxframes].objs[mycontext].data;

  //map to game coordinates
  int upx,upy,upz;
  screen_unproject( i_mousex, i_mousey, ylayer * co->bsy, &upx, &upy, &upz );

  int dnx = downx>=0 ? downx : upx;
  int dny = downy>=0 ? downy : upy;
  int dnz = downz>=0 ? downz : upz;

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

  glPushAttrib(GL_CURRENT_BIT);
  glColor4f(1.0f,1.0f,1.0f,fabsf((float)(vidfr%30)-15.0f)/15.0f);

  SPRITE_T *spr  = sprites + myspr;
  SPRITE_T *dspr = spr;

  for( k=dnz; k<=upz; k++ ) for( j=dny; j<=upy; j++ ) for( i=dnx; i<=upx; i++ ) {
    if( !spr ) continue;

    if( (spr->flags & TOOL_MASK) == TOOL_PSTE && gh && gh->clipboard_data && (upz-dnz||upy-dny||upx-dnx) ) {
      int x = (i-dnx+shx) % clipx;
      int y = (j-dny+shy) % clipy;
      int z = (k-dnz+shz) % clipz;
      dspr = sprites + gh->clipboard_data[ x + y*clipx + z*clipy*clipx ].spr;
    } else if( co->projection == ORTHOGRAPHIC ) {
      dspr = sprite_grid_transform_xy(spr, co, i, j, k, i-dnx, j-dny, upx-dnx+1, upy-dny+1);
    }
                 
    draw_sprite_on_tile( dspr, co, i, j, k );
  }

  glDepthMask( GL_FALSE );
  draw_guides(co,upx,upy,upz);
  glDepthMask( GL_TRUE );
  glPopAttrib();
}

void mod_outerdraw(Uint32 vidfr,int w,int h)
{
  if( !editmode ) return;

  glPushAttrib(GL_CURRENT_BIT);

  int sz = NATIVE_TEX_SZ;

  glBindTexture(GL_TEXTURE_2D,0);
  glColor4f(0.1,0.1,0.1,0.8f);
  SJGL_Blit( &(REC){0,0,sz,sz}, w-sz, 0, 0 );

  SJGL_SetTex( mytex );
  glColor4f(1,1,1,1);
  SJGL_Blit( &(REC){0,0,sz,sz}, w-sz, 0, 0 );

  size_t i;

  glBindTexture(GL_TEXTURE_2D,0);

  // draw sprite boxes
  for( i=0; i<spr_count; i++ ) {
    if( sprites[i].texnum != mytex )
      continue;

    int b1,b2;
    if( myspr==(int)i ) { glColor4f(1,1,0,1.0f); b1 = 4; }
    else                { glColor4f(1,1,1,0.6f); b1 = 1; }
    b2 = b1*2;

    REC rec = sprites[i].rec; 
    int x = w-sz+rec.x;
    int y =      rec.y;

    SJGL_Blit( &(REC){0,0,rec.w+b2,   b1}, x-   b1, y-   b1, 0 );
    SJGL_Blit( &(REC){0,0,rec.w+b2,   b1}, x-   b1, y+rec.h, 0 );
    SJGL_Blit( &(REC){0,0,      b1,rec.h}, x-   b1, y      , 0 );
    SJGL_Blit( &(REC){0,0,      b1,rec.h}, x+rec.w, y      , 0 );
  }

  // draw anchor points
  glColor4f(1,0,0,0.8f);
  for( i=0; i<spr_count; i++ ) {
    if( sprites[i].texnum != mytex )
      continue;

    REC rec = sprites[i].rec; 
    int x = w-sz+rec.x;
    int y =      rec.y;

    if( myspr==(int)i )
      SJGL_Blit( &(REC){0,0,4,4}, x+sprites[i].ancx-2, y+sprites[i].ancy-2, 0 );
    else
      SJGL_Blit( &(REC){0,0,2,2}, x+sprites[i].ancx-1, y+sprites[i].ancy-1, 0 );
  }

  glColor4f(1,1,1,1);
  if( myspr < (int)spr_count ) {
    SJF_DrawText( w-sz, sz+ 4, SJF_LEFT,
                  "Texture #%d \"%s\"", mytex, mytex < (int)tex_count ? textures[mytex].filename : "ERROR! mytex > tex_count" );
    SJF_DrawText( w-sz, sz+14, SJF_LEFT, "Sprite #%d \"%s\"", myspr, sprites[myspr].name );
    SJF_DrawText( w-sz, sz+24, SJF_LEFT, "Layer %d", ylayer );
  }

  glPopAttrib();

  SJF_DrawText(i_mousex+7,i_mousey+15,SJF_LEFT,"%d",ylayer);
}

void mod_adv(int objid,Uint32 a,Uint32 b,OBJ_t *oa,OBJ_t *ob)
{
  switch( ob->type ) {
    case OBJT_MOTHER:
      assert(ob->size==sizeof(MOTHER_t));
      assert(objid==0);
      obj_mother_adv(     objid, a, b, oa, ob );
      break;
    case OBJT_GHOST:
      assert(ob->size==sizeof(GHOST_t));
      obj_ghost_adv(      objid, a, b, oa, ob );
      break;
    case OBJT_PERSON:
      assert(ob->size==sizeof(PERSON_t));
      obj_person_adv(       objid, a, b, oa, ob );
      break;
  } //end switch ob->type
}

static void screen_unproject( int screenx, int screeny, int height, int *x, int *y, int *z )
{
  V ray = get_screen_ray(screenx,v_h-screeny);

  *x = (int)floorf( (v_eyex + (height-v_eyey) * ray.x / ray.y) / 24 );
  *y = (int)ylayer;
  *z = (int)floorf( (v_eyez + (height-v_eyey) * ray.z / ray.y) / 24 );
}

static void draw_sprite_on_tile( SPRITE_T *spr, CONTEXT_t *co, int x, int y, int z )
{
  if( !spr ) return;
  SJGL_SetTex( spr->texnum );

  x = x * co->bsx + co->bsx/2;
  y = y * co->bsy;
  z = z * co->bsz + co->bsz/2;

  if( spr->flags&SPRF_FLOOR )
    SJGL_Box3D( spr, x, y, z );
  else
    SJGL_Wall3D( spr, x, y, z );
}

static int sprite_at(int texnum, int x, int y)
{
  size_t i;

  for( i=0; i<spr_count; i++ ) {
    if( sprites[i].texnum != texnum )
      continue;

    REC rec = sprites[i].rec; 
    if( x >= rec.x && x < rec.x+rec.w &&
        y >= rec.y && y < rec.y+rec.h    )
      return i;
  }

  return -1; 
}
