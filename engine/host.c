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
#include "client.h"
#include "net.h"


UDPsocket hostsock = NULL;


static CLIENT_t *clients;
static UDPpacket *pkt;


void host_start(int port) {
  if(!port) port=HOSTPORT;
  if( hostsock ) { SJC_Write("Already running as a host. Type disconnect to stop."); return; }
  if( clientsock ) { SJC_Write("Already connected to a host. Type disconnect if that ain't cool."); return; }
  if( !(hostsock = SDLNet_UDP_Open(port)) ) { SJC_Write("Error: Could not open host socket!"); SJC_Write(SDL_GetError()); return; }
  clients = calloc(maxclients,sizeof(CLIENT_t));
  me = 0;
  clients[me].connected = 1;
  clients[me].addr = (IPaddress){0,0};
  SJC_Write("Host started on port %u.",port);
  pkt = SDLNet_AllocPacket(PACKET_SIZE);
}


void host_stop() {
  free(clients);
  SDLNet_FreePacket(pkt);
  SDLNet_UDP_Close(hostsock);
  hostsock = NULL;
}


void host() {
  int status;
  int i;
  Uint32 u;
  Uint8 *data;
  size_t n;
  Uint32 packfr;
  Uint32 pktnum;
  FCMD_t *pcmd;

  //recv from clients
  for(;;) {
    status = SDLNet_UDP_Recv(hostsock,pkt);
    if( status==-1 ) {
      SJC_Write("Network Error: Recv failed!");
      SJC_Write(SDL_GetError());
    }
    if( status!=1 )
      break;
    for(i=0;i<maxclients;i++)
      if(clients[i].connected &&
         clients[i].addr.host==pkt->address.host &&
         clients[i].addr.port==pkt->address.port)
        break;
    if(i==maxclients) //a new client is connecting?
      host_welcome();
    else switch(pkt->data[0]) { //clients[i] has something to say!
      case 'm': //message
        SJC_Write("Client %d says: %.*s",i,pkt->len,pkt->data);
        break;
      case 'c': //cmd update
        n = 1;
        pktnum = unpackbytes(pkt->data,pkt->len,&n,4); //FIXME: do something with this!
SJC_Write("Miracle! Packet number %d received from client %d",pktnum,i);
        packfr = unpackbytes(pkt->data,pkt->len,&n,4);
        if( packfr<metafr-30 ) {
          SJC_Write("Ignoring too old cmd from client %d",i);
          break;
        }
        if( packfr>metafr+10 ) {
          SJC_Write("Ignoring too new cmd from client %d",i);
          break;
        }
        setcmdfr(packfr);
        if( hotfr>packfr-1 )
          sethotfr(packfr-1);
        fr[packfr%maxframes].dirty = 1;
        pcmd = fr[packfr%maxframes].cmds+i;
        pcmd->cmd     = unpackbytes(pkt->data,pkt->len,&n,1);
        pcmd->mousehi = unpackbytes(pkt->data,pkt->len,&n,1);
        pcmd->mousex  = unpackbytes(pkt->data,pkt->len,&n,1);
        pcmd->mousey  = unpackbytes(pkt->data,pkt->len,&n,1);
        pcmd->flags   = unpackbytes(pkt->data,pkt->len,&n,2);
        if( pcmd->flags & CMDF_DATA ) { //check for variable data
          pcmd->datasz = unpackbytes(pkt->data,pkt->len,&n,2);
          if( pcmd->datasz > sizeof pcmd->data ) {
            SJC_Write("Treachery: datasz too large (%d) from client %d",pcmd->datasz,i);
            break;
          }
          memcpy( pcmd->data, pkt->data+n, pcmd->datasz );
          n += pcmd->datasz;
        }
        break;
      default:
        SJC_Write("Client %d sent mysterious, incomprehensible packet",i);
        break;
    }
  }

  //send to clients
  pkt->data[0] = 'C';
  pkt->data[1] = 0;
  pkt->len = 2;
  for(u=surefr+1;u<=cmdfr;u++) { //scan for dirty frames to send
    if( fr[u%maxframes].dirty ) {
      data = packframecmds(u,&n);
      if( pkt->len+4+n >= 500 /*pkt->maxlen*/ || pkt->data[1]>100 ) { //FIXME: use a smaller pkt->maxlen
        SJC_Write("%u: Packed too many cmds! Will get the rest next frame...",hotfr);
        free(data);
        break;
      }
      packbytes(pkt->data+pkt->len,u,NULL,4);
      memcpy(pkt->data+pkt->len+4, data, n);
      free(data);
      pkt->len += 4+n;
      pkt->data[1]++;
      fr[u%maxframes].dirty = 0;
    }
  }
  if( pkt->data[1]>0 ) { //we have packed cmds to send along!
    for(i=0;i<maxclients;i++) {
      pkt->address = clients[i].addr;
      if( clients[i].connected && pkt->address.host && !SDLNet_UDP_Send(hostsock,-1,pkt) ) {
        SJC_Write("Error: Could not send cmds packet!");
        SJC_Write(SDL_GetError());
      }
    }
  }
}


//accept a new client and store the "connection"
void host_welcome() {
  int    i;
  Uint32 u;
  size_t n;
  char  *p = (char *)pkt->data;
  Uint8 *q;
  if( strncmp(p,PROTONAME,strlen(PROTONAME)) ) {
    SJC_Write("Junk packet from unknown client at %u:%u.",pkt->address.host,pkt->address.port);
    return;
  }
  p += strlen(PROTONAME);
  if( *p!='/' ) {
    SJC_Write("Malformed packet from unknown client.");
    return;
  }
  p++;
  if( strncmp(p,VERSION,strlen(VERSION)) || pkt->len!=strlen(PROTONAME)+1+strlen(VERSION) ) {
    SJC_Write("Wrong protocol version from unknown client.");
    //TODO: inform client of the problem
    return;
  }
  for(i=0;i<maxclients;i++)
    if( !clients[i].connected ) break;
  if( i==maxclients ) {
    SJC_Write("New client at %u:%u not accepted b/c server is full.",pkt->address.host,pkt->address.port);
    //TODO: inform client
    sprintf((char *)pkt->data,"MFULL: Server is full!");
    pkt->len = strlen((char *)pkt->data)+1;
    SDLNet_UDP_Send(hostsock,-1,pkt);
    return; 
  }
  SJC_Write("New client at %u:%u accepted.",pkt->address.host,pkt->address.port);
  clients[i].connected = 1;
  clients[i].addr = pkt->address;
  setcmdfr(metafr);
  sethotfr(metafr-1);
  fr[metafr%maxframes].dirty = 1;
  fr[metafr%maxframes].cmds[i].flags |= CMDF_NEW;
  // send state!
  q = packframe(surefr,&n);
  SJC_Write("Frame %u packed into %d bytes, ready to send state.",surefr,n);
  if( n+10>(size_t)pkt->maxlen ) {
    SJC_Write("Error: Packed frame is too big to send!");
    free(q);
    return;
  }
  pkt->len = n+10;
  packbytes(pkt->data+0,   'S',NULL,1);
  packbytes(pkt->data+1,     i,NULL,1);
  packbytes(pkt->data+2,metafr,NULL,4);
  packbytes(pkt->data+6,surefr,NULL,4);
  memcpy(pkt->data+10,q,n);
  if( !SDLNet_UDP_Send(hostsock,-1,pkt) ) {
    SJC_Write("Error: Could not send state packet!");
    SJC_Write(SDL_GetError());
    free(q);
    return;
  }
  //dirty all unsure frames
  SJC_Write("%u: Dirtying all frames from %u to %u",hotfr,surefr,cmdfr);
  for(u=surefr+1;u<cmdfr;u++)
    fr[u%maxframes].dirty = 1;

  free(q);

  //TODO: lots
}


