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

#include "SDL.h"
#include "SDL_net.h"
#include "mod.h"
#include "input.h"
#include "main.h"
#include "console.h"
#include "net.h"
#include "video.h"
#include "command.h"
#include "keynames.h"


char *inputdevicenames[] = {"baddevice","keyb","joy","axisp","axisn","mouse"};

int i_mousex = 0;
int i_mousey = 0;

int i_hasmouse = 1;
int i_hasfocus = 1;
int i_minimized = 0;

int i_watch = 0;


static int  kwik = 0;
static char kwik_presscmd;
static char kwik_releasecmd;

static FCMD_t cmdbuf[250];
static int    cbwrite = 0;
static int    cbread = 0;

static SDL_Joystick **joysticks = NULL;

static int started = 0;


void inputinit()
{
  if( started ) {
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    if( SDL_InitSubSystem(SDL_INIT_JOYSTICK) != 0 )
      SJC_Write("Error: could not restart the JOYSTICK SubSystem!");
  }

  init_keynames();

  int i, numjoysticks;
  if( (numjoysticks=SDL_NumJoysticks())>0 ) {
    joysticks = realloc(joysticks, sizeof(*joysticks)*numjoysticks);
    SDL_JoystickEventState(SDL_ENABLE);
    SJC_Write("%d controller/joystick%s detected:",numjoysticks,(numjoysticks>1?"s":""));
    for( i=0; i<numjoysticks; i++ ) {
      joysticks[i] = SDL_JoystickOpen(i);
      SJC_Write("  #%i: %.20s",i,SDL_JoystickName(i));
    }
  }

  started = 1;
}


void putcmd(int device,int sym,int press)
{
  if( cbread%250==(cbwrite+1)%250 ) // full
    return;

  if( mod_mkcmd( cmdbuf+cbwrite, device, sym, press ) )
    return;

  cbwrite = (cbwrite+1)%250;
}


FCMD_t *getnextcmd()
{
  if( cbread==cbwrite )
    return NULL;
  FCMD_t *c = cmdbuf+cbread;
  cbread = (cbread+1)%250;
  return c;
}


void setactive(Uint8 gain,Uint8 state)
{
  if( state & SDL_APPMOUSEFOCUS )
    i_hasmouse = gain;
  if( state & SDL_APPINPUTFOCUS )
    i_hasfocus = gain;
  if( state & SDL_APPACTIVE )
    i_minimized = !gain;
}


void kbinput(int press,SDL_keysym keysym)
{
  SDLKey sym = keysym.sym;
  SDLMod mod = keysym.mod;
  Uint16 unicode = keysym.unicode;

  if( i_watch && press )
    SJC_Write("key #%d, mod #%d, unicode #%d",sym,mod,unicode);

  if( (sym==SDLK_q && mod&(KMOD_CTRL|KMOD_META)) || (sym==SDLK_F4 && mod&KMOD_ALT) )
    command("exit");
  else if( press && (sym==SDLK_F11 || (sym==SDLK_f && mod&KMOD_META)) ) {
    if( !v_fullscreen )
      command("fullscreen");
    else
      command("window");
  } else if(press && sym==SDLK_BACKQUOTE)
    toggleconsole();
  else if(press && kwik)
    kwikbind( INP_KEYB, sym );
  else if(console_open) {
    if(!press)
      ; //nothing on key up
    else if(unicode>31 && unicode<127)
      SJC_Put((char)unicode);
    else if(sym==SDLK_RETURN) {
      if( SJC_Submit() )
        command(SJC.buf[1]);
    }
    else if(sym==SDLK_BACKSPACE || sym==SDLK_DELETE)
      SJC_Rub();
    else if(sym==SDLK_UP)
      SJC_Up();
    else if(sym==SDLK_DOWN)
      SJC_Down();
    else if(sym==SDLK_ESCAPE && console_open)
      toggleconsole();
  } else
    putcmd( INP_KEYB,sym,press );
}


void joyinput(int press,SDL_JoyButtonEvent jbutton)
{
  if( i_watch && press )
    SJC_Write("jbutton #%d",jbutton.button);

  if( kwik && press )
    kwikbind( INP_JBUT, jbutton.button );
  else
    putcmd( INP_JBUT,jbutton.button,press );
}


void axisinput(SDL_JoyAxisEvent jaxis)
{
  static char **axdats = NULL;
  static int size = 0;
  static const char POS_ON  = 1;
  static const char NEG_ON  = 2;

  if( size<=jaxis.which ) //haven't seen a joystick this high before?
  {
    axdats = realloc(axdats,sizeof(*axdats)*(jaxis.which+1));
    memset(axdats+size,0,sizeof(*axdats)*(jaxis.which+1-size));
    size = jaxis.which+1;
  }
  if( !axdats[jaxis.which] ) //haven't seen this exact joystick before?
    axdats[jaxis.which] = calloc(256,sizeof(**axdats));
  char *stat = axdats[jaxis.which]+jaxis.axis;
  int val = jaxis.value;
  int ax = jaxis.axis;

  if( i_watch )
    SJC_Write("joystick #%d, axis #%d, stat %d, value %d",jaxis.which,ax,*stat,val);

  if( val> 11000 && !(*stat&POS_ON) ) { *stat|= POS_ON; kwik ? kwikbind(INP_JAXP,ax) : putcmd( INP_JAXP,ax,1 ); }
  if( val< 10000 &&  (*stat&POS_ON) ) { *stat&=~POS_ON;                                putcmd( INP_JAXP,ax,0 ); }
  if( val<-11000 && !(*stat&NEG_ON) ) { *stat|= NEG_ON; kwik ? kwikbind(INP_JAXN,ax) : putcmd( INP_JAXN,ax,1 ); }
  if( val>-10000 &&  (*stat&NEG_ON) ) { *stat&=~NEG_ON;                                putcmd( INP_JAXN,ax,0 ); }
}


void mouseinput(int press,SDL_MouseButtonEvent mbutton)
{
  if( i_watch && press )
    SJC_Write("mbutton #%d, x %d, y %d",mbutton.button,i_mousex,i_mousey);

  i_mousex = mbutton.x;
  i_mousey = mbutton.y;
  if( kwik )
    kwikbind(INP_MBUT,mbutton.button);
  else
    putcmd( INP_MBUT,mbutton.button,press );
}


void mousemove(SDL_MouseMotionEvent mmotion)
{
  i_mousex = mmotion.x;
  i_mousey = mmotion.y;
}


void readinput()
{
  Uint32 infr = hotfr+1; //TODO: _should_ we always insert on hotfr+1?
  if( cmdfr<infr ) //this is the new cmdfr, so clear it, unless we already have cmds stored in the future!
    setcmdfr(infr);
  infr %= maxframes;
  if( fr[infr].cmds[me].cmd==0 ) {
    FCMD_t *c;
    if( (c = getnextcmd()) ) { // dirty frame if new cmd inserted
      fr[infr].cmds[me] = *c;
      fr[infr].dirty = 1;
    }
  }
}


void input_bindsoon(int presscmd,int releasecmd)
{
  kwik_presscmd = presscmd;
  kwik_releasecmd = releasecmd;
  kwik = 1;
}

void kwikbind(int device,int sym)
{
  mod_keybind( device, sym, 0, kwik_releasecmd );
  mod_keybind( device, sym, 1, kwik_presscmd );
  kwik = 0;
  const char *keyname = sym<KEYNAMECOUNT ? keynames[sym] : NULL;
  switch( device ) {
    case INP_KEYB: SJC_Write("Key #%d \"%s\" selected",  sym,keyname); break;
    case INP_JBUT: SJC_Write("Joy button #%d selected"          ,sym); break;
    case INP_JAXP: SJC_Write("Axis #%d (+) selected"            ,sym); break;
    case INP_JAXN: SJC_Write("Axis #%d (-) selected"            ,sym); break;
    case INP_MBUT: SJC_Write("Mouse button #%d selected"        ,sym); break;
    default:       SJC_Write("Unkown device input #%d selected" ,sym); break;
  }
}



