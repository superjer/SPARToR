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


#include "mod.h"
#include "saveload.h"
#include "sjglob.h"

#define COUNTOF(ident) ((sizeof (ident)) / (sizeof *(ident)))


SYS_TEX_T sys_tex[] = {{"/player.png"     ,0},
                       {"/slugtunnel.png" ,0},
                       {"/amigo.png"      ,0}};
size_t num_sys_tex = COUNTOF(sys_tex);


INPUTNAME_t inputnames[] = {{"left"       ,CMDT_1LEFT ,CMDT_0LEFT },
                            {"right"      ,CMDT_1RIGHT,CMDT_0RIGHT},
                            {"up"         ,CMDT_1UP   ,CMDT_0UP   },
                            {"down"       ,CMDT_1DOWN ,CMDT_0DOWN },
                            {"fire"       ,CMDT_1DASH ,CMDT_0DASH },
                            {"jump"       ,CMDT_1JUMP ,CMDT_0JUMP },
                            {"edit-paint" ,CMDT_1EPANT,CMDT_0EPANT},
                            {"edit-prev"  ,CMDT_1EPREV,CMDT_0EPREV},
                            {"edit-next"  ,CMDT_1ENEXT,CMDT_0ENEXT},
                            {"edit-texdn" ,CMDT_1EPGDN,CMDT_0EPGDN}};
int numinputnames = COUNTOF(inputnames);


char objectnames[][16] =
     { "empty",
       "context",
       "mother",
       "ghost",
       "dummy",
       "player",
       "bullet",
       "slug",
       "amigo",
       "amigosword" };


int    myghost;     //obj number of local player ghost
int    myghostleft; //top left of visible area for local player
int    myghosttop;
int    mycontext;
int    downx = -1; //position of mousedown at beginning of edit cmd
int    downy = -1;

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
static int    mytile   = 0;
static size_t mytex    = 2;


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
  MAYBE_BIND(INP_KEYB,SDLK_z       ,JUMP ); MAYBE_BIND(INP_KEYB,SDLK_SPACE   ,JUMP );
  MAYBE_BIND(INP_KEYB,SDLK_x       ,DASH ); MAYBE_BIND(INP_KEYB,SDLK_LSHIFT  ,DASH );

  MAYBE_BIND(INP_JAXN,0            ,LEFT ); MAYBE_BIND(INP_JAXN,3            ,LEFT ); //joystick or gamepad
  MAYBE_BIND(INP_JAXP,0            ,RIGHT); MAYBE_BIND(INP_JAXP,3            ,RIGHT);
  MAYBE_BIND(INP_JAXN,1            ,UP   ); MAYBE_BIND(INP_JAXN,4            ,UP   );
  MAYBE_BIND(INP_JAXP,1            ,DOWN ); MAYBE_BIND(INP_JAXP,4            ,DOWN );
  MAYBE_BIND(INP_JBUT,0            ,JUMP );
  MAYBE_BIND(INP_JBUT,2            ,DASH );

  MAYBE_BIND(INP_MBUT,1            ,EPANT); //editing controls...
  MAYBE_BIND(INP_MBUT,4            ,EPREV);
  MAYBE_BIND(INP_MBUT,5            ,ENEXT);
  MAYBE_BIND(INP_KEYB,SDLK_PAGEDOWN,EPGDN);
  #undef MAYBE_BIND

  //make the mother object
  fr[setupfr].objs[0] = (OBJ_t){ OBJT_MOTHER, 0, 0, sizeof(MOTHER_t), malloc(sizeof(MOTHER_t)) };
  *(MOTHER_t *)fr[setupfr].objs[0].data = (MOTHER_t){0};

  //make default context object (map)
  fr[setupfr].objs[1] = (OBJ_t){ OBJT_CONTEXT, 0, 0, sizeof(CONTEXT_t), malloc(sizeof(CONTEXT_t)) };
  CONTEXT_t *co = fr[setupfr].objs[1].data;
  co->blocksize = 16;
  co->x = 100;
  co->y =  15;
  co->z =   1;
  int volume = co->x * co->y * co->z;
  co->map  = hack_map  = malloc( (sizeof *co->map ) * volume ); //FIXME remove hack
  co->dmap = hack_dmap = malloc( (sizeof *co->dmap) * volume );
  memset( co->map,  0, (sizeof *co->map ) * volume );
  memset( co->dmap, 0, (sizeof *co->dmap) * volume );
  int i;
  for( i=0; i<volume; i++ ) {
    co->map[ i].data[0] = 255;
    co->map[ i].data[1] = (char)TEX_WORLD;
    co->dmap[i].flags   = CBF_NULL;
  }
  load_context("donnie",1,setupfr); //load a default map

  //make some dummys
  #define MAYBE_A_DUMMY(i,x,y,w,h) {                                                                             \
    DUMMY_t *du;                                                                                                 \
    fr[setupfr].objs[i+20].type = OBJT_DUMMY;                                                                    \
    fr[setupfr].objs[i+20].flags = OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDX|OBJF_BNDB; \
    fr[setupfr].objs[i+20].context = 1;                                                                          \
    fr[setupfr].objs[i+20].size = sizeof *du;                                                                    \
    du = fr[setupfr].objs[i+20].data = malloc(sizeof *du);                                                       \
    du->pos = (V){x*8,y*8,0.0f};                                                                                 \
    du->vel = (V){0.0f,0.0f,0.0f};                                                                               \
    du->hull[0] = (V){-w*8,-h*8,0.0f};                                                                           \
    du->hull[1] = (V){ w*8, h*8,0.0f};                                                                           \
    du->model = 0;                                                                                               }
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

  for(i=0; i<binds_size; i++)
    if( binds[i].hash==hash ) {
      memset( c, 0, sizeof *c );
      c->cmd = binds[i].cmd;

      if( c->cmd==CMDT_0EPREV || c->cmd==CMDT_0ENEXT ) //these shouldn't really happen and wouldn't mean anything
        return -1;

      if( c->cmd==CMDT_1EPREV || c->cmd==CMDT_1ENEXT ) { //select previous/next tile
        if( c->cmd==CMDT_1EPREV )
          mytile += mytile%16==15 ? -15 : 1;
        else
          mytile = (mytile+16)%256;
        return -1;
      }

      if( c->cmd==CMDT_1EPGDN || c->cmd==CMDT_0EPGDN ) { //change current texture
        if( c->cmd==CMDT_0EPGDN )
          mytex = (mytex + 1) % tex_count;
      }

      if( c->cmd==CMDT_1EPANT || c->cmd==CMDT_0EPANT ) { //edit-paint command
        int dnx = downx;
        int dny = downy;
        if( c->cmd==CMDT_0EPANT ) { //always clear mousedown pos on mouseup
          downx = -1;
          downy = -1;
        }
        if( !editmode )
          return -1;
        if( i_mousex >= v_w-256 && i_mousey < 256 ) { //click in texture selector
          mytile = (i_mousex-(v_w-256))/16 + (i_mousey/16)*16;
          return -1;
        }
        int tilex = screen2native_x(i_mousex);
        int tiley = screen2native_y(i_mousey);
        if( !i_hasmouse || tilex<0 || tiley<0 || tilex>=NATIVEW || tiley>=NATIVEH )
          return -1;
        tilex = (myghostleft + tilex) / 16;
        tiley = (myghosttop  + tiley) / 16;
        if( c->cmd==CMDT_1EPANT ) {
          downx = tilex;
          downy = tiley;
          return -1; //finish cmd later...
        }
        if( dnx<0 || dny<0 )
          return -1; //no mousedown? no cmd
        size_t n = 0;
        packbytes(c->data,   'p',&n,1);
        packbytes(c->data,   dnx,&n,4);
        packbytes(c->data,   dny,&n,4);
        packbytes(c->data, tilex,&n,4);
        packbytes(c->data, tiley,&n,4);
        packbytes(c->data,mytile,&n,1);
        packbytes(c->data, mytex,&n,1);
        c->datasz = n;
        c->flags |= CMDF_DATA; //indicate presence of extra cmd data
      }
      return 0; //cmd created
    }
  return -1;
}


int mod_command(char *q)
{
  TRY
    if( q==NULL ){
      ;
    }else if( strcmp(q,"edit")==0 ){
      editmode = editmode ? 0 : 1;
      v_center = editmode ? 0 : 1;
      return 0;
    }else if( strcmp(q,"model")==0 ){
      setmodel = atoi(strtok(NULL," ")); // FIXME: lame hack
      return 0;
    }
  HARDER
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

  SJGLOB_T *files = SJglob( MODNAME "/textures", "*.png", SJGLOB_MARK|SJGLOB_NOESCAPE );

  for( i=0; i<files->gl_pathc; i++ )
    make_sure_texture_is_loaded( files->gl_pathv[i] );

  SJglobfree( files );
}


void mod_predraw(Uint32 vidfr)
{
  int i,j,k;

  //draw context
  CONTEXT_t *co = fr[vidfr%maxframes].objs[1].data; //FIXME: get correct context!
  for( k=0; k<co->z; k++ )
    for( j=0; j<co->y; j++ )
      for( i=0; i<co->x; i++ ) {
        int pos = co->x*co->y*k + co->x*j + i;
        int tile;
        size_t ntex;
        if( co->dmap[ pos ].flags & CBF_NULL ) {
          tile = co->map[  pos ].data[0];
          ntex = co->map[  pos ].data[1];
        } else {
          tile = co->dmap[ pos ].data[0]; 
          ntex = co->dmap[ pos ].data[1];
        }

        SJGL_SetTex( ntex );
        SJGL_Blit( &(SDL_Rect){(tile%16)*16,(tile/16)*16,16,16}, i*16, j*16, 0 );
      }
}


void mod_draw(int objid,OBJ_t *o)
{
  switch(o->type) {
    case OBJT_PLAYER:         obj_player_draw(     objid, o );     break;
    case OBJT_GHOST:          obj_ghost_draw(      objid, o );     break;
    case OBJT_BULLET:         obj_bullet_draw(     objid, o );     break;
    case OBJT_SLUG:           obj_slug_draw(       objid, o );     break;
    case OBJT_DUMMY:          obj_dummy_draw(      objid, o );     break;
    case OBJT_AMIGO:          obj_amigo_draw(      objid, o );     break;
    case OBJT_AMIGOSWORD:     obj_amigosword_draw( objid, o );     break;
  }
}


void mod_postdraw(Uint32 vidfr)
{
  int i,j;
  int upx = screen2native_x(i_mousex);
  int upy = screen2native_y(i_mousey);

  if( !editmode || !i_hasmouse || upx<0 || upy<0 || upx>=NATIVEW || upy>=NATIVEH ) return;

  upx = (myghostleft + upx) / 16;
  upy = (myghosttop  + upy) / 16;

  int dnx = downx>=0 ? downx : upx;
  int dny = downy>=0 ? downy : upy;

  if( dnx > upx )  { int tmp = upx; upx = dnx; dnx = tmp; } //make so dn is less than up
  if( dny > upy )  { int tmp = upy; upy = dny; dny = tmp; }

  glPushAttrib(GL_CURRENT_BIT);
  glColor4f(1.0f,1.0f,1.0f,fabsf((float)(vidfr%30)-15.0f)/15.0f);
  SJGL_SetTex( mytex );

  int tile = mytile;
  GHOST_t *gh = myghost ? fr[vidfr%maxframes].objs[myghost].data : NULL;
  for( j=dny; j<=upy; j++ )
    for( i=dnx; i<=upx; i++ ) {
      if( mytile==0x5F && gh && gh->clipboard_data && (upy-dny||upx-dnx) ) // PSTE tool texture
        tile = gh->clipboard_data[ ((j-dny)%gh->clipboard_y)*gh->clipboard_x + ((i-dnx)%gh->clipboard_x) ].data[0];
      SJGL_Blit( &(SDL_Rect){(tile%16)*16,(tile/16)*16,16,16}, i*16, j*16, NATIVEH );
    }

  glPopAttrib();
}


void mod_outerdraw(Uint32 vidfr,int w,int h)
{
  if( !editmode ) return;

  glPushAttrib(GL_CURRENT_BIT);

  SJGL_SetTex( mytex );
  SJGL_Blit( &(SDL_Rect){0,0,256,256}, w-256, 0, 0 );

  glBindTexture(GL_TEXTURE_2D,0);
  glColor4f(1,1,0,0.8f);
  int x = w-256+(mytile%16)*16;
  int y = (mytile/16)*16;
  SJGL_Blit( &(SDL_Rect){0,0,20, 2}, x- 2, y- 2, 0 );
  SJGL_Blit( &(SDL_Rect){0,0,20, 2}, x- 2, y+16, 0 );
  SJGL_Blit( &(SDL_Rect){0,0, 2,16}, x- 2, y   , 0 );
  SJGL_Blit( &(SDL_Rect){0,0, 2,16}, x+16, y   , 0 );

  SJF_DrawText( w-256, 260, mytex < tex_count ? textures[mytex].filename : "ERROR! mytex > tex_count" );

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

