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
#include "main.h"
#include "console.h"
#include "command.h"
#include "host.h"
#include "hostbox.h"
#include "client.h"
#include "pack.h"

// TODO Move everything after to a different file:
void host_send_state(int clientid)
{
  // TODO: write to client outbox rather than sending packet directly
  size_t     n;
  Uint32     u;
  Uint8     *data;
  UDPpacket *pkt;

  pkt = SDLNet_AllocPacket(80000);

  setcmdfr(metafr);
  sethotfr(metafr-1);
  fr[metafr%maxframes].dirty = 1;
  fr[metafr%maxframes].cmds[clientid].flags |= CMDF_NEW;
  // send state!
  data = packframe(surefr, &n);
  SJC_Write("Frame %u packed into %d bytes, ready to send state.", surefr, n);

  if( n+10 > (size_t)pkt->maxlen ) //TODO is this just PACKET_SIZE?
  {
    SJC_Write("Error: Packed frame is too big to send!");
    goto cleanup;
  }

  pkt->len = n+10;
  packbytes(pkt->data+0,      'S', NULL, 1);
  packbytes(pkt->data+1, clientid, NULL, 1);
  packbytes(pkt->data+2,   metafr, NULL, 4);
  packbytes(pkt->data+6,   surefr, NULL, 4);
  memcpy(pkt->data+10, data, n);

  if( !SDLNet_UDP_Send(hostsock, -1, pkt) )
  {
    SJC_Write("Error: Could not send state packet!");
    SJC_Write(SDL_GetError());
    goto cleanup;
  }

  //dirty all unsure frames
  SJC_Write("%u: Dirtying all frames from %u to %u", hotfr, surefr, cmdfr);

  for(u=surefr+1;u<cmdfr;u++)
    fr[u%maxframes].dirty = 1;

  cleanup:
  free(data);
  SDLNet_FreePacket(pkt);
}

void host_outbox_write()
{
  Uint32  u;
  Uint8  *data;
  size_t  n;
  int     framecount = 0;

  //send to clients
  Uint8 outbox[80000];
  outbox[0] = 'C';
  outbox[1] = 0;
  outbox[2] = 0;
  size_t outbox_len = 3;

  for( u=surefr+1; u<=cmdfr; u++ ) //scan for dirty frames to send
  {
    if( fr[u%maxframes].dirty )
    {
      data = packframecmds(u, &n);

      if( outbox_len + 4 + n >= 80000 )
      {
        SJC_Write("%u: Packed too many cmds! Will get the rest next frame...", hotfr);
        free(data);
        break;
      }

      packbytes(outbox, u, &outbox_len, 4);
      memcpy(outbox + outbox_len, data, n);
      outbox_len += n;

      free(data);
      framecount++;
      fr[u%maxframes].dirty = 0;
    }
  }

  // store number of frames written back at the top of the buffer
  packbytes(outbox + 1, framecount, NULL, 2);

  if( !framecount )
    outbox_len = 0;
}

void host_inbox_read(int clientid, CLIENT_t *cl)
{
  FCMD_t *pcmd;
  size_t n = 0;

  if( !cl->connected || !cl->buflen )
    return;

  SJC_Write("Inbox from client %d has %d bytes", clientid, cl->buflen);

  Uint32 packfr = unpackbytes(cl->buf, cl->buflen, &n, 4);

  if( packfr<metafr-30 )
  {
    SJC_Write("Ignoring too old cmd from client %d", clientid);
    return;
  }

  if( packfr>metafr+10 ) // check for ring buffer safety!
  {
    SJC_Write("Ignoring too new cmd from client %d", clientid);
    return;
  }

  setcmdfr(packfr); // this can NOT be right!!!!!

  if( hotfr>packfr-1 ) // this either!!!
    sethotfr(packfr-1);

  fr[packfr%maxframes].dirty = 1;
  pcmd = fr[packfr%maxframes].cmds+clientid;
  pcmd->cmd     = unpackbytes(cl->buf, cl->buflen, &n, 1);
  pcmd->mousehi = unpackbytes(cl->buf, cl->buflen, &n, 1);
  pcmd->mousex  = unpackbytes(cl->buf, cl->buflen, &n, 1);
  pcmd->mousey  = unpackbytes(cl->buf, cl->buflen, &n, 1);
  pcmd->flags   = unpackbytes(cl->buf, cl->buflen, &n, 2);

  if( pcmd->flags & CMDF_DATA ) //check for variable data
  {
    pcmd->datasz = unpackbytes(cl->buf, cl->buflen, &n, 2);

    if( pcmd->datasz > sizeof pcmd->data )
    {
      SJC_Write("Treachery: datasz too large (%d) from client %d", pcmd->datasz, clientid);
      return;
    }

    memcpy(pcmd->data, cl->buf+n, pcmd->datasz);
    n += pcmd->datasz;
  }
}
