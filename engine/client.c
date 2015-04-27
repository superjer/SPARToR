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
#include "main.h"
#include "console.h"
#include "command.h"
#include "client.h"
#include "host.h"
#include "pack.h"

static Uint32 pktnum;
static Uint32 sentfr = 0;
static int negotiated;
static UDPpacket *pkt;

void client()
{
  int status;
  int i;
  size_t n;
  Uint32 packfr;

  return;

  //send cmd updates to server
  while(negotiated && sentfr<metafr-1) {
    sentfr++;
    Uint32 sentfrmod = sentfr%maxframes;
    if( fr[sentfrmod].dirty ) {
      n = 0;
      FCMD_t *c = fr[sentfrmod].cmds+me;
      packbytes(pkt->data,'c'             ,&n,1);
      packbytes(pkt->data,pktnum++        ,&n,4);
      packbytes(pkt->data,sentfr          ,&n,4);
      packbytes(pkt->data,c->cmd          ,&n,1);
      packbytes(pkt->data,c->mousehi      ,&n,1);
      packbytes(pkt->data,c->mousex       ,&n,1);
      packbytes(pkt->data,c->mousey       ,&n,1);
      packbytes(pkt->data,c->flags        ,&n,2);
      if( c->flags & CMDF_DATA ) {
        packbytes(pkt->data,c->datasz     ,&n,2);
        memcpy(pkt->data+n,c->data,c->datasz);
        n += c->datasz;
      }
      pkt->len = n;
      if( !SDLNet_UDP_Send(clientsock,-1,pkt) ) {
        echo("Error: Could not send cmd update packet!");
        echo(SDL_GetError());
      }
      fr[sentfrmod].dirty = 0; //TODO: really share this var?
    }
  }

  //look for data from server
  for(;;)
  {
    status = SDLNet_UDP_Recv(clientsock,pkt);

    if( status==-1 )
    {
      echo("Network Error: Failed to check for new packets.");
      echo(SDL_GetError());
    }

    if( status!=1 )
      break;

    switch(pkt->data[0])
    {
      case 'M': //message
        echo("Server says: %s",pkt->data+1);
        break;

      case 'S': //state
        clearframebuffer();
        me               = unpackbytes(pkt->data+1,4,NULL,1);
        Uint32 newmetafr = unpackbytes(pkt->data+2,4,NULL,4);
        Uint32 newsurefr = unpackbytes(pkt->data+6,4,NULL,4);
        sentfr = newmetafr-1;
        jogframebuffer( newmetafr, newsurefr );
        echo("Receiving state of frame %d, %d bytes, syncing up at frame %d as client %d",
                  surefr,pkt->len-10,metafr,me);

        unpackframe(surefr,pkt->data+10,pkt->len-10);
        negotiated = 1;
        break;

      case 'C': //cmds
        n = 2;
        for(i=0; i<(int)pkt->data[1]; i++)
        {
          packfr = unpackbytes(pkt->data,pkt->len,&n,4);
          setcmdfr(packfr);
          int unpacked = unpackframecmds(packfr,pkt->data+n,pkt->len-n);

          if( unpacked<0 )
          {
            echo("Failed to unpack frame cmds!");
            break;
          }

          n += unpacked;

          if( hotfr>packfr-1 )
            sethotfr(packfr-1);
        }
        break;

      default:
        echo("Error: Packet is garbled!");
    }
  }
}
