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

#include <limits.h>
#include <unistd.h>

#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"
#include "SDL_net.h"
#include "mod.h"
#include "main.h"
#include "console.h"
#include "command.h"
#include "pack.h"
#include "net.h"
#include "host.h"
#include "client.h"
#include "video.h"
#include "audio.h"
#include "input.h"
#include "saveload.h"
#include "sprite.h"
#include "keynames.h"
#include "helpers.h"
#include "xorshift.h"

int netmode;

static void bind( char *dev_sym, char *press_cmdname );

void command(const char *s)
{
        char *p, *q;
        char buf[strlen(s)+1];

        strcpy(buf, s);
        q = p = buf;

        while( *q )
        {
                if( isspace(*q) ) *q = ' ';
                q++;
        }

        q = tok(p, " ");

        if( q==NULL )
        {
                ;
        }
        else if( strcmp(q, "xorshift")==0 )
        {
                echo("%lld", xorshift128plus());
        }
        else if( strcmp(q, "console")==0 )
        {
                toggleconsole();
        }
        else if( strcmp(q, "exec")==0 )
        {
                char *file = tok(p, " ");
                if( !file ) { echo("You must specify a name in " GAME "/console/*.txt"); return; }
                exec_commands(file);
        }
        else if( strcmp(q, "realtime")==0 )
        {
                eng_realtime = eng_realtime ? 0 : 1;
                echo("Realtime mode %s", eng_realtime?"on":"off");
        }
        else if( strcmp(q, "watch")==0 )
        {
                i_watch = i_watch ? 0 : 1;
                echo("Input watch %s", i_watch?"on":"off");
        }
        else if( strcmp(q, "exit")==0 || strcmp(q, "quit")==0 )
        {
                cleanup();
        }
        else if( strcmp(q, "listen")==0 )
        {
                char *port = tok(p, " ");
                int nport = port ? atoi(port) : HOSTPORT;
                int err = net_start(nport, 8);

                if( !err )
                {
                        echo("Started host on port %d", nport);
                        netmode = NM_HOST;
                }
        }
        else if( strcmp(q, "connect")==0 )
        {
                char *hostname = tok(p, " :");
                char *port = tok(p, " :");
                char *clientport = tok(p, " ");
                int nport = port ? atoi(port) : HOSTPORT;
                int nclientport = clientport ? atoi(clientport) : CLIENTPORT;
                int err = net_start(nclientport, 1);

                if( !err )
                {
                        if( !hostname ) hostname = "localhost";

                        err = net_connect(hostname, nport);
                        if( !err )
                        {
                                echo("Connecting to %s on port %d", hostname, nport);
                                netmode = NM_CLIENT;
                        }
                }
        }
        else if( strcmp(q, "echo")==0 )
        {
                if( p && *p )
                {
                        int ret = net_write(0, (unsigned char*)p, strlen(p));
                        if( ret ) echo("Net write error: %d", ret);
                }
        }
        else if( strcmp(q, "resend")==0 )
        {
                net_resend = 1;
        }
        else if( strcmp(q, "disconnect")==0 )
        {
                if( netmode == NM_HOST )
                {
                        net_stop();
                        echo("Host stopped.");
                }
                else if( netmode == NM_CLIENT )
                {
                        net_stop();
                        echo("Disconnected from host.");
                }
                else
                {
                        echo("Nothing to disconnect from.");
                }
                netmode = NM_NONE;

        }
        else if( strcmp(q, "reconnect")==0 )
        {
                echo("Not implemented.");
        }
        else if( strcmp(q, "log")==0 )
        {
                echo("Switching to log file \"%s\"", p);
                console_log(p);
        }
        else if( strcmp(q, "hulls")==0 )
        {
                v_drawhulls = v_drawhulls ? 0 : 1;
        }
        else if( strcmp(q, "stats")==0 )
        {
                v_showstats = v_showstats ? 0 : 1;
        }
        else if( strcmp(q, "oscillo")==0 )
        {
                v_oscillo = v_oscillo ? 0 : 1;
        }
        else if( strcmp(q, "musictest")==0 )
        {
                v_oscillo = a_musictest = a_musictest ? 0 : 1;
        }
        else if( strcmp(q, "fullscreen")==0 || strncmp(q, "window", 6)==0 )
        {
                char *sw = tok(p, " x");
                char *sh = tok(p, " ");
                int w = sw ? atoi(sw) : 0;
                int h = sh ? atoi(sh) : 0;

                if( strcmp(q, "fullscreen")==0 )
                {
                        setvideosoon(0, 0, 1);
                }
                else
                {
                        if( w>=320 && h>=200 )
                        {
                                SDL_SetWindowSize(screen, w, h);
                                setvideosoon(w, h, 0);
                        }
                        else if( w>=1 && w<=5 )
                        {
                                SDL_SetWindowSize(screen, NATIVEW*w, NATIVEH*w);
                                setvideosoon(NATIVEW*w, NATIVEH*w, 0);
                        }
                        else
                        {
                                setvideosoon(0, 0, 0);
                        }
                }
        }
        else if( strcmp(q, "winpos")==0 )
        {
                char *sx = tok(p, " ");
                char *sy = tok(p, " ");
                int x = sx ? atoi(sx) : -1;
                int y = sy ? atoi(sy) : -1;

                if( x < 0 || y < 0 )
                        echo("Please specify x and y position");
                else
                        setwinpos(x, y);
        }
        else if( strcmp(q, "bind")==0 )
        {
                char *arg0 = tok(p, " ");
                char *arg1 = tok(p, "\0");
                if( arg1 )
                {
                        char *s = arg1 + strlen(arg1);
                        while( --s>arg1 && isspace(*s) )
                                *s = '\0';
                }
                bind( arg0, arg1 );
        }
        else if( strcmp(q, "slow")==0 )
        {
                echo("Speed is now slow");
                ticksaframe = 300000;
                jogframebuffer(metafr, surefr);
        }
        else if( strcmp(q, "fast")==0 )
        {
                echo("Speed is now fast");
                ticksaframe = 30000;
                jogframebuffer(metafr, surefr);
        }
        else if( strcmp(q, "help")==0 )
        {
                echo("Press your ~ key to open and close this console. Commands you can type:");
                echo("     \\#08Flisten               \\#FFFstart a server");
                echo("     \\#08Fconnect              \\#FFFconnect to a server");
                echo("     \\#08Ffullscreen           \\#FFFgo fullscreen");
                echo("     \\#08Ffullscreen 1024 768  \\#FFFgo fullscreen at 1024x768");
                echo("     \\#08Fwindow 3x            \\#FFFgo windowed at 3x up-scale");
                echo("     \\#08Fbind                 \\#FFFchoose input keys");
                echo("See commands.txt for more commands");
        }
        else if( strcmp(q, "report")==0 )
        {
                int i;
                for( i=0; i<maxobjs; i++ )
                {
                        object *o = fr[surefr%maxframes].objs+i;
                        if( o->type )
                                echo( "#%-3i %-20s C:%-3i F:%-5x", i, flexer[o->type].name, o->context, o->flags );
                }
        }
        else if( strcmp(q, "save")==0 )
        {
                char *name = tok(p, " ");
                if( name==NULL )
                {
                        echo("Please specify a file name to save");
                        return;
                }
                save_context( name, mycontext, hotfr );
        }
        else if( strcmp(q, "load")==0 )
        {
                char *name = tok(p, " ");
                if( name==NULL )
                {
                        echo("Please specify a file name to load");
                        return;
                }
                load_context( name, mycontext, hotfr );
        }
        else if( strcmp(q, "spr")==0 )
        {
                char *num = tok(p, " ");
                if( num==NULL )
                {
                        echo("There are %d sprites", spr_count);
                        return;
                }

                size_t n = atoi(num);
                if( n >= spr_count )
                {
                        echo("Invalid sprite number #%d (max %d)", n, spr_count-1);
                        return;
                }

                echo("Sprite #%d \"%s\"  texture %d \"%s\" flags %d",
                        n, sprites[n].name, sprites[n].texnum, textures[sprites[n].texnum].filename, sprites[n].flags);
                echo("  pos %d %d  size %d %d  anchor %d %d",
                        sprites[n].rec.x, sprites[n].rec.y, sprites[n].rec.w, sprites[n].rec.h, sprites[n].ancx, sprites[n].ancy);
                echo("  floor %d  flange %d  bump %d",
                        sprites[n].flags&SPRF_FLOOR, sprites[n].flange, sprites[n].bump);

                if( sprites[n].more )
                        echo("  gridwide %d  gridlast %d  piping %d  stretch %d (%d %d %d %d)",
                                sprites[n].more->gridwide, sprites[n].more->gridlast,
                                sprites[n].more->piping, sprites[n].more->stretch,
                                sprites[n].more->stretch_t, sprites[n].more->stretch_r, sprites[n].more->stretch_b, sprites[n].more->stretch_l);
        }
        else if( strcmp(q, "fovy")==0 )
        {
                char *num = tok(p, " ");
                if( num==NULL )
                {
                        echo("fovy is %f, eyedist is %d", v_fovy, v_eyedist);
                        return;
                }
                float n = atof(num);
                if( n < 0.0001f || n > 90.0f )
                {
                        echo("Value out of range (0.0001-90)");
                        return;
                }
                v_fovy = n;
                return;
        }
        else if( strcmp(q, "pwd")==0 || strcmp(q, "cwd")==0 )
        {
                char buf[PATH_MAX+1];
                echo("%s", getcwd(buf, PATH_MAX));
        }
        else if( strcmp(q, "cd")==0 || strcmp(q, "chdir")==0 )
        {
                char buf[PATH_MAX+1];
                if( !p || !*p )
                        echo("Please specify a path");
                else if( chdir(p) )
                        echo("Failed to change path to: %s", p);
                else
                        echo("Changed path to: %s", getcwd(buf, PATH_MAX));
        }
        else if( strcmp(q, "fontscale")==0 )
        {
                float scale = atof(p);
                if( scale < 1 || scale > 9 )
                        echo("Font scale must be between 1 and 9, sir.");
                else
                        v_conscale = scale;
        }
        else if( mod_command(q, p) )
        {
                echo("%s: command not found", q);

        }
}

static void parse_dev_sym( int *devnum, int *sym, char *dev_sym )
{
        *devnum = INP_JBUT;
        for( *sym=0; *sym<PADNAMECOUNT; (*sym)++ )
                if( padnames[*sym] && 0==strcmp(padnames[*sym], dev_sym) )
                        return;

        *devnum = INP_KEYB;
        for( *sym=0; *sym<KEYNAMECOUNT; (*sym)++ )
                if( keynames[*sym] && 0==strcmp(keynames[*sym], dev_sym) )
                        return;

        *sym = atoi(dev_sym);
        if( *sym )
                return;

        for( *devnum=0; *devnum<=INP_MAX; (*devnum)++ )
        {
                char *p = inputdevicenames[*devnum];
                int plen = strlen(p);
                if( 0==strncmp(p, dev_sym, plen) )
                {
                        *sym = atoi(dev_sym+plen);
                        return;
                }
        }

        *devnum = 0;
}

static void bind( char *dev_sym, char *press_cmdname )
{
        char *cmdname;
        int press;
        int device = 0;
        int sym;
        int cmd;

        if( dev_sym==NULL )
        {
                mod_showbinds();
                return;
        }

        if( press_cmdname==NULL )
        {
                cmdname = dev_sym;
        }
        else
        {
                parse_dev_sym(&device, &sym, dev_sym);

                if( !device ) { echo("Unrecognized key, button, or stick: %s", dev_sym); return; }

                if(      press_cmdname[0]=='+' ) { press = 1; cmdname = press_cmdname+1; }
                else if( press_cmdname[0]=='-' ) { press = 0; cmdname = press_cmdname+1; }
                else                             { press =-1; cmdname = press_cmdname;   }
        }

        if( device && cmdname[0]=='!' )
        {
                mod_keybind( device, sym, 1, 0, cmdname+1 );
                return;
        }

        for( cmd=0; cmd<numinputnames; cmd++ )
                if( 0==strcmp(inputnames[cmd].name, cmdname) )
                        break;

        if( cmd==numinputnames ) { echo("Not a command: %s", cmdname); return; }

        if( !device )
        {
                input_bindsoon( inputnames[cmd].presscmd, inputnames[cmd].releasecmd );
                echo("Press a key, button, or stick to use for [%s] ...", cmdname);
                return;
        }

        if( press!=0 ) mod_keybind( device, sym, 1, inputnames[cmd].presscmd  , NULL );
        if( press!=1 ) mod_keybind( device, sym, 0, inputnames[cmd].releasecmd, NULL );
}

void exec_commands( char *name )
{
        char path[PATH_MAX];
        int printed;
        FILE *f;
        char line[1000];

        printed = snprintf( path, PATH_MAX, GAME "/console/%s.txt", name );
        if( printed<0 ) { echo("Error making path from %s", path); return; }

        f = fopen(path, "r");
        if( !f ) { echo("Couldn't open %s", path); return; }

        while( fgets(line, 1000, f) )
                command(line);

        fclose(f);

        // FIXME LAME HACK FOR NOW
        printed = snprintf( path, PATH_MAX, "user/console/%s.txt", name );
        if( printed<0 ) { echo("Error making path from %s", path); return; }

        f = fopen(path, "r");
        if( !f ) { echo("Couldn't open %s", path); return; }

        while( fgets(line, 1000, f) )
                command(line);

        fclose(f);
}
