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


SYS_TEX_T sys_tex[] = {{"/tool.png"       ,0},
                       {"/player.png"     ,0},
                       {"/slugtunnel.png" ,0},
                       {"/amigo.png"      ,0},
                       {"/persons.png"    ,0},
                       {"/blankhud.png"   ,0}};
size_t num_sys_tex = COUNTOF(sys_tex);


INPUTNAME_t inputnames[] = {{"left"       ,CMDT_1LEFT ,CMDT_0LEFT },
                            {"right"      ,CMDT_1RIGHT,CMDT_0RIGHT},
                            {"up"         ,CMDT_1UP   ,CMDT_0UP   },
                            {"down"       ,CMDT_1DOWN ,CMDT_0DOWN },
                            {"nw"         ,CMDT_1NW   ,CMDT_0NW   },
                            {"ne"         ,CMDT_1NE   ,CMDT_0NE   },
                            {"sw"         ,CMDT_1SW   ,CMDT_0SW   },
                            {"se"         ,CMDT_1SE   ,CMDT_0SE   },
                            {"select"     ,CMDT_1SEL  ,CMDT_0SEL  },
                            {"back"       ,CMDT_1BACK ,CMDT_0BACK },
                         /* {"cons-cmd"   ,CMDT_1CON  ,CMDT_0CON  }, this may not be necessary, it may even be dangerous */
                            {"edit-paint" ,CMDT_1EPANT,CMDT_0EPANT},
                            {"edit-prev"  ,CMDT_1EPREV,CMDT_0EPREV},
                            {"edit-next"  ,CMDT_1ENEXT,CMDT_0ENEXT},
                            {"edit-texup" ,CMDT_1EPGUP,CMDT_0EPGUP},
                            {"edit-texdn" ,CMDT_1EPGDN,CMDT_0EPGDN},
                            {"edit-lay0"  ,CMDT_1ELAY0,CMDT_0ELAY0},
                            {"edit-lay1"  ,CMDT_1ELAY1,CMDT_0ELAY1},
                            {"edit-lay2"  ,CMDT_1ELAY2,CMDT_0ELAY2}};
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
  short hash;
  char  cmd;
  char  _pad;
}            *binds;
static int    editmode = 0;
static int    myspr    = 0;
static int    mytex    = 0;
static FCMD_t magic_c;      // magical storage for an extra command, triggered from console


static void draw_sprite_on_tile( SPRITE_T *spr, CONTEXT_t *co, int x, int y, int z );
static int sprite_at(int texnum, int x, int y);


void mod_setup(Uint32 setupfr)
{
  //default key bindings
  #define MAYBE_BIND(dev,sym,cmd)         \
    mod_keybind(dev,sym,0,CMDT_0 ## cmd); \
    mod_keybind(dev,sym,1,CMDT_1 ## cmd);
  MAYBE_BIND(INP_KEYB,SDLK_LEFT    ,LEFT ); MAYBE_BIND(INP_KEYB,SDLK_a       ,LEFT ); //keyboard
  MAYBE_BIND(INP_KEYB,SDLK_RIGHT   ,RIGHT); MAYBE_BIND(INP_KEYB,SDLK_d       ,RIGHT);
  MAYBE_BIND(INP_KEYB,SDLK_UP      ,UP   ); MAYBE_BIND(INP_KEYB,SDLK_w       ,UP   );
  MAYBE_BIND(INP_KEYB,SDLK_DOWN    ,DOWN ); MAYBE_BIND(INP_KEYB,SDLK_s       ,DOWN );
  MAYBE_BIND(INP_KEYB,SDLK_a       ,SEL  ); MAYBE_BIND(INP_KEYB,SDLK_SPACE   ,SEL  );
  MAYBE_BIND(INP_KEYB,SDLK_s       ,BACK );

  // keypad directions!
  MAYBE_BIND(INP_KEYB,SDLK_KP7     ,NW   ); MAYBE_BIND(INP_KEYB,SDLK_KP8     ,UP   ); MAYBE_BIND(INP_KEYB,SDLK_KP9     ,NE   );
  MAYBE_BIND(INP_KEYB,SDLK_KP4     ,LEFT );                                           MAYBE_BIND(INP_KEYB,SDLK_KP6     ,RIGHT);
  MAYBE_BIND(INP_KEYB,SDLK_KP1     ,SW   ); MAYBE_BIND(INP_KEYB,SDLK_KP2     ,DOWN ); MAYBE_BIND(INP_KEYB,SDLK_KP3     ,SE   );

  MAYBE_BIND(INP_JAXN,0            ,LEFT ); MAYBE_BIND(INP_JAXN,3            ,LEFT ); //joystick or gamepad
  MAYBE_BIND(INP_JAXP,0            ,RIGHT); MAYBE_BIND(INP_JAXP,3            ,RIGHT);
  MAYBE_BIND(INP_JAXN,1            ,UP   ); MAYBE_BIND(INP_JAXN,4            ,UP   );
  MAYBE_BIND(INP_JAXP,1            ,DOWN ); MAYBE_BIND(INP_JAXP,4            ,DOWN );
  MAYBE_BIND(INP_JBUT,1            ,SEL  );
  MAYBE_BIND(INP_JBUT,2            ,BACK );

  MAYBE_BIND(INP_MBUT,1            ,EPANT); //editing controls...
  MAYBE_BIND(INP_MBUT,4            ,EPREV);
  MAYBE_BIND(INP_MBUT,5            ,ENEXT);
  MAYBE_BIND(INP_KEYB,SDLK_PAGEDOWN,EPGDN);
  MAYBE_BIND(INP_KEYB,SDLK_PAGEUP  ,EPGUP);
  MAYBE_BIND(INP_KEYB,SDLK_0       ,ELAY0);
  MAYBE_BIND(INP_KEYB,SDLK_1       ,ELAY1);
  MAYBE_BIND(INP_KEYB,SDLK_2       ,ELAY2);
  #undef MAYBE_BIND

  //make the mother object
  fr[setupfr].objs[0] = (OBJ_t){ OBJT_MOTHER, 0, 0, sizeof(MOTHER_t), malloc(sizeof(MOTHER_t)) };
  *(MOTHER_t *)fr[setupfr].objs[0].data = (MOTHER_t){0,{0,0,0,0,0,0}};

  //make default context object (map)
  fr[setupfr].objs[1] = (OBJ_t){ OBJT_CONTEXT, 0, 0, sizeof(CONTEXT_t), malloc(sizeof(CONTEXT_t)) };
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

  //make some dummys
  #define MAYBE_A_DUMMY(i,x,y,w,h) {                                                         \
    DUMMY_t *du;                                                                             \
    fr[setupfr].objs[i+20].type = OBJT_DUMMY;                                                \
    fr[setupfr].objs[i+20].flags = OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP| \
                                   OBJF_BNDX|OBJF_BNDZ|OBJF_BNDB;                            \
    fr[setupfr].objs[i+20].context = 1;                                                      \
    fr[setupfr].objs[i+20].size = sizeof *du;                                                \
    du = fr[setupfr].objs[i+20].data = malloc(sizeof *du);                                   \
    du->pos = (V){x*8,y*8,0};                                                                \
    du->vel = (V){0,0,0};                                                                    \
    du->hull[0] = (V){-w*8,-h*8,-8};                                                         \
    du->hull[1] = (V){ w*8, h*8, 8};                                                         \
    du->model = 0;                                                                           }
  MAYBE_A_DUMMY(20,  3,-25,1,1);
  MAYBE_A_DUMMY(21,  3,-20,1,1);
  MAYBE_A_DUMMY(22,  3,-15,1,1);
  MAYBE_A_DUMMY(23,  3,-10,1,1);
  MAYBE_A_DUMMY(24,  5,-15,1,1);
  MAYBE_A_DUMMY(25,  9,-15,1,1);
  MAYBE_A_DUMMY(26, 43,-20,1,1);
  MAYBE_A_DUMMY(27, 43,-15,1,1);
  MAYBE_A_DUMMY(28, 45,-25,1,1);
  MAYBE_A_DUMMY(29, 45,-20,1,1);
  MAYBE_A_DUMMY(30, 45,-15,1,1);
  #undef MAYBE_A_DUMMY

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


void mod_keybind(int device,int sym,int press,char cmd)
{
  int i;
  short hash = press<<15 | device<<8 | sym;

  for(i=0; i<binds_size; i++)
    if( binds[i].hash==hash || binds[i].hash==0 )
      break;
  if(i==binds_size) {
    binds = realloc(binds,sizeof(*binds)*(binds_size+32));
    memset(binds+binds_size,0,sizeof(*binds)*32);
    binds_size += 32;
  }
  binds[i].hash = hash;
  binds[i].cmd = cmd;
}


// returns 0 iff a command is created to be put on the network
int mod_mkcmd(FCMD_t *c,int device,int sym,int press)
{
  int i;
  short hash = press<<15 | device<<8 | sym;
  CONTEXT_t *co = fr[hotfr%maxframes].objs[mycontext].data;

  // apply magic command?
  if( !sym ) {
    if( magic_c.datasz ) {
      memcpy(c, &magic_c, sizeof magic_c);
      memset(&magic_c, 0, sizeof magic_c);
      return 0;
    }
    return -1;
  }

  for(i=0; i<binds_size; i++)
    if( binds[i].hash==hash ) {
      memset( c, 0, sizeof *c );
      c->cmd = binds[i].cmd;

      if( !editmode )
        return 0;

      if( c->cmd==CMDT_0EPREV || c->cmd==CMDT_0ENEXT ) //these shouldn't really happen and wouldn't mean anything
        return -1;

      if( c->cmd==CMDT_1EPREV ) { //select previous tile
        myspr = (myspr + spr_count - 1) % spr_count;
        mytex = sprites[myspr].texnum;
        return -1;
      }

      if( c->cmd==CMDT_1ENEXT ) { //select next tile
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

      if( c->cmd==CMDT_0ELAY0 || c->cmd==CMDT_0ELAY1 || c->cmd==CMDT_0ELAY2 ) {
        showlayer = 0;
        return -1;
      }
      if( c->cmd==CMDT_1ELAY0 ) { ylayer = 0; showlayer = 1; return -1; }
      if( c->cmd==CMDT_1ELAY1 ) { ylayer = 1; showlayer = 1; return -1; }
      if( c->cmd==CMDT_1ELAY2 ) { ylayer = 2; showlayer = 1; return -1; }

      if( c->cmd==CMDT_1EPANT || c->cmd==CMDT_0EPANT ) { //edit-paint command
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

        int posx = screen2native_x(i_mousex);
        int posy = screen2native_y(i_mousey);

        if( co->projection == DIMETRIC ) {
          posx += co->tileuw/2; // tiles are centered in width
          posy -= co->bsy*co->y; // tiles are drawn at the bottom of the cube
        }

        //map to game coordinates
        int tilex = NATIVE2TILE_X(co,posx,posy);
        int tiley = NATIVE2TILE_Y(co,posx,posy);
        int tilez = NATIVE2TILE_Z(co,posx,posy);

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
      }
      return 0; //cmd created
    }
  return -1;
}


int safe_atoi(const char *s)
{
  if( !s ) return 0;
  return atoi(s);
}


int mod_command(char *q)
{
  if( q==NULL ){
    ;

  }else if( strcmp(q,"edit")==0 ){
    editmode = editmode ? 0 : 1;
    v_center = editmode ? 0 : 1;
    return 0;

  }else if( strcmp(q,"model")==0 ){
    setmodel = safe_atoi(strtok(NULL," ")); // FIXME: lame hack
    return 0;

  }else if( strcmp(q,"bounds")==0 || strcmp(q,"blocksize")==0 ){
    size_t n = 0;
    int x = safe_atoi(strtok(NULL," "));
    int y = safe_atoi(strtok(NULL," "));
    int z = safe_atoi(strtok(NULL," "));
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
    magic_c.cmd = CMDT_0CON; // secret command type for doing crap like this!
    putcmd(0,0,0);
    return 0;

  }else if( strcmp(q,"tilespacing")==0 ){
    size_t n = 0;
    int tileuw = safe_atoi(strtok(NULL," "));
    int tileuh = safe_atoi(strtok(NULL," "));

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
    putcmd(0,0,0);
    return 0;

  }else if( strcmp(q,"orthographic")==0 || strcmp(q,"dimetric")==0 ){
    size_t n = 0;

    memset(&magic_c,0,sizeof magic_c);
    packbytes(magic_c.data,q[0],&n,1);
    magic_c.datasz = n;
    magic_c.flags |= CMDF_DATA;
    magic_c.cmd = CMDT_0CON;
    putcmd(0,0,0);
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

  if( co->projection == ORTHOGRAPHIC )
    PROJECTION_MODE(ORTHO);
  else
    PROJECTION_MODE(DIMETRIC);

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
    case OBJT_PLAYER:         obj_player_draw(     objid, vidfrmod, o, co );     break;
    case OBJT_GHOST:          obj_ghost_draw(      objid, vidfrmod, o, co );     break;
    case OBJT_BULLET:         obj_bullet_draw(     objid, vidfrmod, o, co );     break;
    case OBJT_SLUG:           obj_slug_draw(       objid, vidfrmod, o, co );     break;
    case OBJT_PERSON:         obj_person_draw(     objid, vidfrmod, o, co );     break;
    case OBJT_DUMMY:          obj_dummy_draw(      objid, vidfrmod, o, co );     break;
    case OBJT_AMIGO:          obj_amigo_draw(      objid, vidfrmod, o, co );     break;
    case OBJT_AMIGOSWORD:     obj_amigosword_draw( objid, vidfrmod, o, co );     break;
  }
}


void mod_huddraw(Uint32 vidfr)
{
  SJGL_SetTex( sys_tex[TEX_HUD].num  );
  SJGL_Blit( &(REC){0,0,160,50},   0, NATIVEH-50, 0 );
  SJGL_Blit( &(REC){0,0,160,50}, 160, NATIVEH-50, 0 );
  SJGL_Blit( &(REC){0,0,160,50}, 320, NATIVEH-50, 0 );

  MOTHER_t *mo = fr[vidfr%maxframes].objs[0].data;

  int i;
  for( i=0; i<6; i++ ) {
    if( !mo->party[i] )
      continue;

    PERSON_t *pe = fr[vidfr%maxframes].objs[mo->party[i]].data;

    #define BAR_W(stat) (pe->stat>0 ? 15+32*pe->stat/pe->max_##stat : 0)
    SJGL_Blit( &(REC){0,50+6*0,BAR_W(hp),6}, 57   , NATIVEH-50+13+9*0, 0 );
    SJGL_Blit( &(REC){0,50+6*1,BAR_W(mp),6}, 57+51, NATIVEH-50+13+9*0, 0 );
    SJGL_Blit( &(REC){0,50+6*2,BAR_W(st),6}, 57   , NATIVEH-50+13+9*1, 0 );
    SJGL_Blit( &(REC){0,50+6*3,BAR_W(ap),6}, 57+51, NATIVEH-50+13+9*1, 0 );
    SJGL_Blit( &(REC){0,50+6*4,BAR_W(pn),6}, 57   , NATIVEH-50+13+9*2, 0 );
    SJGL_Blit( &(REC){0,50+6*5,BAR_W(ml),6}, 57+51, NATIVEH-50+13+9*2, 0 );
    SJGL_Blit( &(REC){0,50+6*6,BAR_W(to),6}, 57   , NATIVEH-50+13+9*3, 0 );
    SJGL_Blit( &(REC){0,50+6*7,BAR_W(xp),6}, 57+51, NATIVEH-50+13+9*3, 0 );
    #undef BAR_W
  }
}


void mod_postdraw(Uint32 vidfr)
{
  int i,j,k;
  int posx = screen2native_x(i_mousex);
  int posy = screen2native_y(i_mousey);

  if( !editmode || !i_hasmouse ) return;

  GHOST_t   *gh = fr[vidfr%maxframes].objs[myghost].data; // FIXME is myghost/mycontext always set here?
  CONTEXT_t *co = fr[vidfr%maxframes].objs[mycontext].data;

  if( co->projection == DIMETRIC ) {
    posx += co->tileuw/2; // tiles are centered in width
    posy -= co->bsy*co->y; // tiles are drawn at the bottom of the cube
  }

  //map to game coordinates
  int upx = NATIVE2TILE_X(co,posx,posy);
  int upy = NATIVE2TILE_Y(co,posx,posy);
  int upz = NATIVE2TILE_Z(co,posx,posy);

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
  SJF_DrawText( w-sz, sz+ 4, SJF_LEFT,
                "Texture #%d \"%s\"", mytex, mytex < (int)tex_count ? textures[mytex].filename : "ERROR! mytex > tex_count" );
  SJF_DrawText( w-sz, sz+14, SJF_LEFT, "Sprite #%d \"%s\"", myspr, sprites[myspr].name );
  SJF_DrawText( w-sz, sz+24, SJF_LEFT, "Layer %d", ylayer );

  glPopAttrib();
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
    case OBJT_DUMMY:
      assert(ob->size==sizeof(DUMMY_t));
      obj_dummy_adv(      objid, a, b, oa, ob );
      break;
    case OBJT_PLAYER:
      assert(ob->size==sizeof(PLAYER_t));
      obj_player_adv(     objid, a, b, oa, ob );
      break;
    case OBJT_BULLET:
      assert(ob->size==sizeof(BULLET_t));
      obj_bullet_adv(     objid, a, b, oa, ob );
      break;
    case OBJT_SLUG:
      assert(ob->size==sizeof(SLUG_t));
      obj_slug_adv(       objid, a, b, oa, ob );
      break;
    case OBJT_PERSON:
      assert(ob->size==sizeof(PERSON_t));
      obj_person_adv(       objid, a, b, oa, ob );
      break;
    case OBJT_AMIGO:
      assert(ob->size==sizeof(AMIGO_t));
      obj_amigo_adv(      objid, a, b, oa, ob );
      break;
    case OBJT_AMIGOSWORD:
      assert(ob->size==sizeof(AMIGOSWORD_t));
      obj_amigosword_adv( objid, a, b, oa, ob );
      break;
  } //end switch ob->type
}


static void draw_sprite_on_tile( SPRITE_T *spr, CONTEXT_t *co, int x, int y, int z )
{
  if( co->projection == DIMETRIC )
    y = co->y * co->bsy; // layers are all anchored at the bottom of the context

  SJGL_SetTex( spr->texnum );

  int c = TILE2NATIVE_X(co,x,y,z);
  int d = TILE2NATIVE_Y(co,x,y,z);
  int r = d;

  if( spr->ancy > co->tileuh ) // for sprites that pop out of the flat plane
    r += co->tileuh / 2;

  // the sprite has an explicit anchor point, which is aligned with the anchor point of the tile,
  // which always in the southernmost corner
  SJGL_Blit( &spr->rec, c - spr->ancx, d + co->tileuh - spr->ancy, r );
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
