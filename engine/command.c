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

#include <limits.h>

#include "SDL.h"
#include "SDL_net.h"
#include "mod.h"
#include "main.h"
#include "console.h"
#include "command.h"
#include "net.h"
#include "host.h"
#include "client.h"
#include "video.h"
#include "audio.h"
#include "input.h"
#include "saveload.h"
#include "sprite.h"
#include "keynames.h"

static void bind( char *dev_sym, char *press_cmdname );

void command(const char *s)
{
  char *p, *q;

  q = p = malloc(strlen(s)+1);
  strcpy(p,s);

  while( *q ) {
    if( isspace(*q) ) *q = ' ';
    q++;
  }

  q = strtok(p," ");

  do {
    if( q==NULL ) {
      ;

    }else if( strcmp(q,"exec")==0 ) {
      char *file = strtok(NULL," ");
      if( !file ) { SJC_Write("You must specify a name in game/console/*.txt"); return; }
      exec_commands(file);

    }else if( strcmp(q,"redetect")==0 ) {
      inputinit();

    }else if( strcmp(q,"realtime")==0 ) {
      eng_realtime = eng_realtime ? 0 : 1;
      SJC_Write("Realtime mode %s",eng_realtime?"on":"off");

    }else if( strcmp(q,"watch")==0 ) {
      i_watch = i_watch ? 0 : 1;
      SJC_Write("Input watch %s",i_watch?"on":"off");

    }else if( strcmp(q,"exit")==0 || strcmp(q,"quit")==0 ) {
      cleanup();

    }else if( strcmp(q,"listen")==0 ) {
      char *port = strtok(NULL," ");
      host_start(port?atoi(port):0);

    }else if( strcmp(q,"connect")==0 ) {
      char *hostname = strtok(NULL," :");
      char *port = strtok(NULL," :");
      char *clientport = strtok(NULL," ");
      client_start(hostname,(port?atoi(port):0),(clientport?atoi(clientport):0));

    }else if( strcmp(q,"disconnect")==0 ) {
      if( hostsock ) {
        host_stop();
        SJC_Write("Host stopped.");
      }else if( clientsock ) {
        client_stop();
        SJC_Write("Disconnected from host.");
      }else
        SJC_Write("Nothing to disconnect from.");

    }else if( strcmp(q,"reconnect")==0 ) {
      SJC_Write("Not implemented."); //TODO

    }else if( strcmp(q,"hulls")==0 ) {
      v_drawhulls = v_drawhulls ? 0 : 1;

    }else if( strcmp(q,"stats")==0 ) {
      v_showstats = v_showstats ? 0 : 1;

    }else if( strcmp(q,"oscillo")==0 ) {
      v_oscillo = v_oscillo ? 0 : 1;

    }else if( strcmp(q,"musictest")==0 ) {
      v_oscillo = a_musictest = a_musictest ? 0 : 1;

    }else if( strcmp(q,"fullscreen")==0 || strncmp(q,"window",6)==0 ) {
      char *sw = strtok(NULL," x");
      char *sh = strtok(NULL," ");
      int w = sw?atoi(sw):0;
      int h = sh?atoi(sh):0;
      int full = strcmp(q,"fullscreen")==0 ? 1 : 0;
      if( w>=320 && h>=200 )
        setvideosoon(w,h,full,1);
      else if( w>=1 && w<=5 )
        setvideosoon(NATIVEW*w,NATIVEH*w,full,1);
      else
        setvideosoon(0,0,full,1);

    }else if( strcmp(q,"bind")==0 ) {
      char *arg0 = strtok(NULL," ");
      char *arg1 = strtok(NULL," ");
      bind( arg0, arg1 );

    }else if( strcmp(q,"slow")==0 ) {
      SJC_Write("Speed is now slow");
      ticksaframe = 300;
      jogframebuffer(metafr,surefr);

    }else if( strcmp(q,"fast")==0 ) {
      SJC_Write("Speed is now fast");
      ticksaframe = 30;
      jogframebuffer(metafr,surefr);

    }else if( strcmp(q,"help")==0 ) {
      SJC_Write("Press your ~ key to open and close this console. Commands you can type:");
      SJC_Write("     \\#08Flisten               \\#FFFstart a server");
      SJC_Write("     \\#08Fconnect              \\#FFFconnect to a server");
      SJC_Write("     \\#08Ffullscreen           \\#FFFgo fullscreen");
      SJC_Write("     \\#08Ffullscreen 1024 768  \\#FFFgo fullscreen at 1024x768");
      SJC_Write("     \\#08Fwindow 3x            \\#FFFgo windowed at 3x up-scale");
      SJC_Write("     \\#08Fbind                 \\#FFFchoose input keys");
      SJC_Write("See commands.txt for more commands");

    }else if( strcmp(q,"report")==0 ) {
      int i;
      for( i=0; i<maxobjs; i++ ) {
        OBJ_t *o = fr[surefr%maxframes].objs+i;
        if( o->type )
          SJC_Write( "#%-3i %-20s C:%-3i F:%-5x", i, flexer[o->type].name, o->context, o->flags );
      }

    }else if( strcmp(q,"save")==0 ) {
      char *name = strtok(NULL," ");
      if( name==NULL ) {
        SJC_Write("Please specify a file name to save");
        break;
      }
      save_context( name, mycontext, hotfr );

    }else if( strcmp(q,"load")==0 ) {
      char *name = strtok(NULL," ");
      if( name==NULL ) {
        SJC_Write("Please specify a file name to load");
        break;
      }
      load_context( name, mycontext, hotfr );

    }else if( strcmp(q,"spr")==0 ) {
      char *num = strtok(NULL," ");
      if( num==NULL ) {
        SJC_Write("There are %d sprites",spr_count);
        break;
      }
      size_t n = atoi(num);
      if( n >= spr_count ) {
        SJC_Write("Invalid sprite number #%d (max %d)",n,spr_count-1);
        break;
      }
      SJC_Write("Sprite #%d \"%s\"  texture %d \"%s\" flags %d",
                n,sprites[n].name,sprites[n].texnum,textures[sprites[n].texnum].filename,sprites[n].flags);
      SJC_Write("  pos %d %d  size %d %d  anchor %d %d",
                sprites[n].rec.x,sprites[n].rec.y,sprites[n].rec.w,sprites[n].rec.h,sprites[n].ancx,sprites[n].ancy);
      SJC_Write("  floor %d  flange %d  bump %d",
                sprites[n].flags&SPRF_FLOOR,sprites[n].flange,sprites[n].bump);
      if( sprites[n].more )
        SJC_Write("  gridwide %d  gridlast %d  piping %d  stretch %d (%d %d %d %d)",
                  sprites[n].more->gridwide,sprites[n].more->gridlast,
                  sprites[n].more->piping,sprites[n].more->stretch,
                  sprites[n].more->stretch_t,sprites[n].more->stretch_r,sprites[n].more->stretch_b,sprites[n].more->stretch_l);

    }else if( strcmp(q,"fovy")==0 ) {
      char *num = strtok(NULL," ");
      if( num==NULL ) {
        SJC_Write("fovy is %f, eyedist is %d",v_fovy,v_eyedist);
        break;
      }
      float n = atof(num);
      if( n < 0.0001f || n > 90.0f ) {
        SJC_Write("Value out of range (0.0001-90)");
        break;
      }
      v_fovy = n;
      break;

    }else if( mod_command(q) ) {
      SJC_Write("Huh?");

    }
  } while(0);

  free(p);
}

static void parse_dev_sym( int *devnum, int *sym, char *dev_sym )
{
  *devnum = INP_KEYB;

  for( *sym=0; *sym<KEYNAMECOUNT; (*sym)++ )
    if( keynames[*sym] && 0==strcmp(keynames[*sym],dev_sym) )
      return;

  *sym = atoi(dev_sym);
  if( *sym )
    return;

  for( *devnum=0; *devnum<=INP_MAX; (*devnum)++ ) {
    char *p = inputdevicenames[*devnum];
    int plen = strlen(p);
    if( 0==strncmp(p,dev_sym,plen) ) {
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

  if( dev_sym==NULL ) {
    mod_showbinds();
    return;
  }

  if( press_cmdname==NULL )
    cmdname = dev_sym;
  else {
    parse_dev_sym(&device,&sym,dev_sym);

    if( !device ) { SJC_Write("Unrecognized key, button, or stick!"); return; }

    if(      press_cmdname[0]=='+' ) { press = 1; cmdname = press_cmdname+1; }
    else if( press_cmdname[0]=='-' ) { press = 0; cmdname = press_cmdname+1; }
    else                             { press =-1; cmdname = press_cmdname;   }
  }

  for( cmd=0; cmd<numinputnames; cmd++ )
    if( 0==strcmp(inputnames[cmd].name,cmdname) )
      break;

  if( cmd==numinputnames ) { SJC_Write("Not a command: %s",cmdname); return; }

  if( !device ) {
    input_bindsoon( inputnames[cmd].presscmd, inputnames[cmd].releasecmd );
    SJC_Write("Press a key, button, or stick to use for [%s] ...",cmdname);
    return;
  }

  if( press!=0 ) mod_keybind( device, sym, 1, inputnames[cmd].presscmd   );
  if( press!=1 ) mod_keybind( device, sym, 0, inputnames[cmd].releasecmd );
}

void exec_commands( char *name )
{
  char path[PATH_MAX];
  int printed;
  FILE *f;
  char line[1000];

  printed = snprintf( path, PATH_MAX, "game/console/%s.txt", name );
  if( printed<0 ) { SJC_Write("Error making path from %s",path); return; }

  f = fopen(path, "r");
  if( !f ) { SJC_Write("Couldn't open %s",path); return; }

  while( fgets(line,1000,f) )
    command(line);

  fclose(f);

  // FIXME LAME HACK FOR NOW
  printed = snprintf( path, PATH_MAX, "user/console/%s.txt", name );
  if( printed<0 ) { SJC_Write("Error making path from %s",path); return; }

  f = fopen(path, "r");
  if( !f ) { SJC_Write("Couldn't open %s",path); return; }

  while( fgets(line,1000,f) )
    command(line);

  fclose(f);
}
