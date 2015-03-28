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

UDPsocket hostsock = NULL;

static CLIENT_t *clients;
static UDPpacket *pkt;

//TODO send everything in the outbox to all the clients
void host_write_packets()
{
  int i;
 
  /* if( !outbox_len ) */
  /*   return; */

  //TODO break up into smaller packets as needed
  //TODO hang on to sent packets in case resend is needed
  /* memcpy(pkt->data, outbox, outbox_len); */
  /* outbox_len = 0; */

  for( i=0; i<maxclients; i++ )
  {
    pkt->address = clients[i].addr;

    if( !clients[i].connected || !pkt->address.host )
      continue;

    if( !SDLNet_UDP_Send(hostsock, -1, pkt) )
    {
      SJC_Write("Error: Could not send cmds packet!");
      SJC_Write(SDL_GetError());
    }
  }
}

void host_read_packet(int clientid)
{
  size_t n = 0;
  int conga = unpackbytes(pkt->data, pkt->len, &n, 4);

  if( conga != clients[clientid].lastconga + 1 )
    return; // silently drop packet on the floor

  size_t payloadlen = unpack(4);

  if( payloadlen != pkt->len - n )
  {
    SJC_Write("Packet length & payloadlen do not match");
    return;
  }

  if( payloadlen > clients[clientid].maxlen - clients[clientid].buflen )
  {
    SJC_Write("Not enough room to store payload");
    return;
  }

  memcpy(
    clients[clientid].buf + clients[clientid].buflen,
    pkt->data,
    payloadlen
  );

  clients[clientid].buflen += payloadlen;
}
