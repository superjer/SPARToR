
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "input.h"
#include "main.h"
#include "console.h"
#include "net.h"
#include "command.h"


static char cmdbuf[256] = {0};
static int cbwrite = 0;
static int cbread = 0;


void putcmd(char cmd) {
  cmdbuf[cbwrite] = cmd;
  cbwrite = (cbwrite+1)%256;
}


char getnextcmd() {
  char cmd = cmdbuf[cbread];
  if( cmd==0 ) return 0;
  cmdbuf[cbread] = 0;
  cbread = (cbread+1)%256;
  return cmd;
}


void input(int press,int sym,Uint16 unicode) {

  if(press && sym==SDLK_BACKQUOTE)
    toggleconsole();
  else if(press && console_open) {
    if(unicode>31 && unicode<128)
      SJC_Put((char)unicode);
    else if(sym==SDLK_RETURN) {
      if( SJC_Submit() )
        command(SJC.buf[1]);
    }
    else if(sym==SDLK_BACKSPACE)
      SJC_Rub();
    else if(sym==SDLK_ESCAPE && console_open)
      toggleconsole();
  } else switch(sym) {
  case SDLK_LEFT:  putcmd( press?CMDT_1LEFT :CMDT_0LEFT  ); break;
  case SDLK_RIGHT: putcmd( press?CMDT_1RIGHT:CMDT_0RIGHT ); break;
  case SDLK_UP:    putcmd( press?CMDT_1UP:CMDT_0UP       ); break;
  case SDLK_DOWN:  putcmd( press?CMDT_1DOWN:CMDT_0DOWN   ); break;
  }
}


void readinput() {
  Uint32 infr = hotfr+1;
  if( cmdfr<infr ) { //this is the new cmdfr, so clear it, unless we already have cmds stored in the future!
                     //TODO: jog the simulation forward if cmds do end up in the future because that must mean we're BEHIND SCHEDULE!
    memset(fr[infr%maxframes].cmds,0,sizeof(FCMD_t)*maxclients);
    cmdfr = infr;
  }
  infr %= maxframes;
  if( fr[infr].cmds[me].cmd==0 && cmdbuf[cbread] ) {
    int i;
    char s[257];
    char t[257];
    for(i=0;i<256;i++) {
      s[i] = cmdbuf[i]?cmdbuf[i]+'0':'-';
      t[i] = ' ';
    }
    t[cbread] = 'R';
    t[cbwrite] = 'W';
    s[256] = '\0';
    t[256] = '\0';
    SJC_Write(s);
    SJC_Write(t);
    fr[infr].cmds[me].cmd = getnextcmd();
  }
}

