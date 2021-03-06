/**
 **  SPARToR
 **  Network Game Engine
 **  Copyright (C) 2010-2015  Jer Wilson
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
#include "mod.h"
#include "input.h"
#include "main.h"
#include "console.h"
#include "pack.h"
#include "video.h"
#include "command.h"
#include "keynames.h"

char *inputdevicenames[] = {"baddevice", "keyb", "joy", "axisp", "axisn", "mouse"};

int i_mousex = 0;
int i_mousey = 0;

int i_hasmouse = 1;
int i_hasfocus = 0;
int i_minimized = 0;

int i_watch = 0;

static SDL_GameController *pads[MAX_PADS];

static int  kwik = 0;
static char kwik_presscmd;
static char kwik_releasecmd;

static FCMD_t cmdbuf[250];
static int    cbwrite = 0;
static int    cbread = 0;

/* static SDL_Joystick **joysticks = NULL; */

void inputinit()
{
        init_keynames();

        /*
        int i, numjoysticks;
        if( (numjoysticks=SDL_NumJoysticks())>0 )
        {
                joysticks = realloc(joysticks, sizeof(*joysticks)*numjoysticks);
                SDL_JoystickEventState(SDL_ENABLE);
                echo("%d controller/joystick%s detected:", numjoysticks, (numjoysticks>1?"s":""));
                for( i=0; i<numjoysticks; i++ )
                {
                        joysticks[i] = SDL_JoystickOpen(i);
                        echo("  #%i: %.20s", i, SDL_JoystickName(i));
                }
        }
        */
}

void putcmd(int device, int sym, int press)
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

void winevent(SDL_WindowEvent e)
{
        switch( e.event )
        {
        case SDL_WINDOWEVENT_ENTER       : i_hasmouse = 1;  break;
        case SDL_WINDOWEVENT_LEAVE       : i_hasmouse = 0;  break;
        case SDL_WINDOWEVENT_FOCUS_GAINED: i_hasfocus = 1;  break;
        case SDL_WINDOWEVENT_FOCUS_LOST  : i_hasfocus = 0;  break;
        case SDL_WINDOWEVENT_MINIMIZED   : i_minimized = 1; break;
        case SDL_WINDOWEVENT_RESTORED    :
        case SDL_WINDOWEVENT_MAXIMIZED   : i_minimized = 0; break;

        case SDL_WINDOWEVENT_RESIZED:
                if( !v_fullscreen )
                        setvideosoon(e.data1, e.data2, 0);
                break;
        }
}

void textinput(SDL_TextInputEvent e)
{
        if( e.text[0] > 31 && e.text[0] < 127 )
                console_put(e.text[0]);
}

void textedit(SDL_TextEditingEvent e)
{
        echo("textedit: %s", e.text);
}

void kbinput(int press, SDL_KeyboardEvent e)
{
        if( e.repeat && !console_open )
                return;

        SDL_Keycode sym = e.keysym.sym;
        SDL_Keymod mod = e.keysym.mod;

        // deal with SDL2 crazy sym codes :(
        int safesym = sym <= 127 ? sym : (sym & (~(1<<30))) + 256;
        int ctrl = mod & KMOD_CTRL;
        int alt  = mod & KMOD_ALT;
        int meta = mod & KMOD_GUI;

        if( i_watch && press )
                echo("key #%i, %s %s %s", safesym, ctrl?"ctrl":"", alt?"alt":"", meta?"meta":"");

        if( (sym==SDLK_q && (ctrl || meta)) || (sym==SDLK_F4 && alt) )
        {
                command("exit");
        }
        else if( press && (sym==SDLK_F11 || (sym==SDLK_f && meta)) )
        {
                if( !v_fullscreen )
                        command("fullscreen");
                else
                        command("window");
        }
        else if( press && sym==SDLK_BACKQUOTE && !alt )
        {
                toggleconsole();
        }
        else if( press && kwik )
        {
                kwikbind( INP_KEYB, safesym );
        }
        else if( console_open )
        {
                char *scmd;
                if( !press ) ; //nothing on key up
                else if( sym==SDLK_ESCAPE                           ) toggleconsole();
                else if( sym==SDLK_RETURN && (scmd=console_submit())) command(scmd);
                else if( sym==SDLK_BACKSPACE                        ) console_rub(0);
                else if( sym==SDLK_DELETE                           ) console_rub(1);
                else if( sym==SDLK_UP    || (sym==SDLK_p && ctrl)   ) console_up();
                else if( sym==SDLK_DOWN  || (sym==SDLK_n && ctrl)   ) console_down();
                else if( sym==SDLK_LEFT  || (sym==SDLK_h && ctrl)   ) console_left();
                else if( sym==SDLK_RIGHT || (sym==SDLK_l && ctrl)   ) console_right();
                else if( sym==SDLK_HOME  || (sym==SDLK_a && ctrl)   ) console_home();
                else if( sym==SDLK_END   || (sym==SDLK_e && ctrl)   ) console_end();
                else if( sym==SDLK_COPY  || (sym==SDLK_c && ctrl)   ) console_copy();
                else if( sym==SDLK_PASTE || (sym==SDLK_v && ctrl)   ) console_paste();
        }
        else
        {
                putcmd( INP_KEYB, safesym, press );
        }
}

void padadd(SDL_ControllerDeviceEvent e)
{
        if( !SDL_IsGameController(e.which) )
                return;

        if( e.which > MAX_PADS )
        {
                echo("padadd: Gamepad ID is too high: %d", e.which);
                return;
        }

        if( !(pads[e.which] = SDL_GameControllerOpen(e.which)) )
        {
                echo("Failed to add gamepad #%d", e.which);
                echo("%s", SDL_GetError());
                return;
        }

        echo("Opened gamepad #%d", e.which);
}

void padremap(SDL_ControllerDeviceEvent e)
{
        // when the hell does this happen and what does it mean??!?!?
        echo("-----> SURPRISE!! padremap: %d", e.which);
}

void padremove(SDL_ControllerDeviceEvent e)
{
        if( e.which > MAX_PADS )
        {
                echo("padremove: Gamepad ID is too high: %d", e.which);
                return;
        }

        if( !pads[e.which] )
        {
                echo("padremove: Not a gamepad: %d", e.which);
                return;
        }

        SDL_GameControllerClose(pads[e.which]);
        pads[e.which] = NULL;
}

void padinput(int press, SDL_ControllerButtonEvent e)
{
        if( i_watch && press )
        {
                const char *padname = e.button<PADNAMECOUNT ? padnames[e.button] : "unknown";
                echo("pad #%d \"%s\"", e.button, padname);
        }

        if( kwik && press )
                kwikbind(INP_JBUT, e.button);
        else
                putcmd(INP_JBUT, e.button, press);
}

void padaxis(SDL_ControllerAxisEvent e)
{
        static char **axdats = NULL;
        static int size = 0;
        static const char POS_ON  = 1;
        static const char NEG_ON  = 2;

        if( size<=e.which ) //haven't seen a joystick this high before?
        {
                axdats = realloc(axdats, sizeof(*axdats)*(e.which+1));
                memset(axdats+size, 0, sizeof(*axdats)*(e.which+1-size));
                size = e.which+1;
        }
        if( !axdats[e.which] ) //haven't seen this exact joystick before?
                axdats[e.which] = calloc(256, sizeof(**axdats));
        char *stat = axdats[e.which]+e.axis;
        int val = e.value;
        int ax = e.axis;

        if( i_watch )
                echo("joystick #%d, axis #%d, stat %d, value %d", e.which, ax, *stat, val);

        if( val> 21000 && !(*stat&POS_ON) ) { *stat|= POS_ON; kwik ? kwikbind(INP_JAXP, ax) : putcmd( INP_JAXP, ax, 1 ); }
        if( val< 20000 &&  (*stat&POS_ON) ) { *stat&=~POS_ON;                                 putcmd( INP_JAXP, ax, 0 ); }
        if( val<-21000 && !(*stat&NEG_ON) ) { *stat|= NEG_ON; kwik ? kwikbind(INP_JAXN, ax) : putcmd( INP_JAXN, ax, 1 ); }
        if( val>-20000 &&  (*stat&NEG_ON) ) { *stat&=~NEG_ON;                                 putcmd( INP_JAXN, ax, 0 ); }
}

void mouseinput(int press, SDL_MouseButtonEvent mbutton)
{
        if( i_watch && press )
                echo("mbutton #%d, x %d, y %d", mbutton.button, i_mousex, i_mousey);

        i_mousex = mbutton.x;
        i_mousey = mbutton.y;

        if( kwik )
                kwikbind(INP_MBUT, mbutton.button);
        else
                putcmd( INP_MBUT, mbutton.button, press );
}

void mousemove(SDL_MouseMotionEvent mmotion)
{
        i_mousex = mmotion.x;
        i_mousey = mmotion.y;
}

void readinput()
{
        unsigned int infr = hotfr+1; //TODO: _should_ we always insert on hotfr+1?

        if( cmdfr<infr ) //this is the new cmdfr, so clear it, unless we already have cmds stored in the future!
                setcmdfr(infr);

        infr %= maxframes;

        if( fr[infr].cmds[me].cmd==0 )
        {
                FCMD_t *c;
                if( (c = getnextcmd()) ) // dirty frame if new cmd inserted
                {
                        fr[infr].cmds[me] = *c;
                        fr[infr].dirty = 1;
                }
        }
}

void input_bindsoon(int presscmd, int releasecmd)
{
        kwik_presscmd = presscmd;
        kwik_releasecmd = releasecmd;
        kwik = 1;
}

void kwikbind(int device, int sym)
{
        mod_keybind( device, sym, 0, kwik_releasecmd, NULL );
        mod_keybind( device, sym, 1, kwik_presscmd  , NULL );
        kwik = 0;

        const char *keyname = sym>=0 && sym<KEYNAMECOUNT ? keynames[sym] : "unknown";
        const char *padname = sym>=0 && sym<PADNAMECOUNT ? padnames[sym] : "unknown";

        switch( device )
        {
                case INP_KEYB: echo("Key #%d \"%s\" selected",  sym, keyname); break;
                case INP_JBUT: echo("Pad #%d \"%s\" selected",  sym, padname); break;
                case INP_JAXP: echo("Axis #%d (+) selected"            , sym); break;
                case INP_JAXN: echo("Axis #%d (-) selected"            , sym); break;
                case INP_MBUT: echo("Mouse button #%d selected"        , sym); break;
                default:       echo("Unknown device input #%d selected", sym); break;
        }
}
