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
#include "main.h"
#include "net.h"
#include "pack.h"
#include "console.h"

enum CONN_STATES {
  CONN_FREE = 0,
  CONN_NEW,
  CONN_CONNECTED,
};

static UDPsocket  sock = NULL;
static UDPpacket *pkt;
static CONNEX_t  *conns;
static int        maxconns;

static int get_free_conn();
static void create_ring(RING_t *r, int count);
static void create_part(int connexid, int partid, int count, Uint8 *data, size_t len);
static void welcome();
static void acknowlege(int connexid);
static void accept_from(int connexid);

int net_start(int port, int _maxconns)
{
  if( sock )
  {
    SJC_Write("Error: Socket already open");
    return -1;
  }

  sock = SDLNet_UDP_Open(port);

  if( !sock )
  {
    SJC_Write("Unable to open port %d", port);
    SJC_Write("%s", SDL_GetError());
    return -1;
  }

  maxconns = _maxconns;
  conns = calloc(sizeof *conns, maxconns);
  pkt = SDLNet_AllocPacket(PACKET_SIZE);
  return 0;
}

void net_stop()
{
  if( !sock ) return;

  SDLNet_UDP_Close(sock);
  sock = NULL;
  SDLNet_FreePacket(pkt);
}

int net_connect(const char *hostname, int port)
{
  IPaddress ipaddr;
  int connexid;

  if( (connexid = get_free_conn()) < 0 )
  {
    SJC_Write("Error: No free connection");
    return -1;
  }

  SDLNet_ResolveHost(&ipaddr, hostname, port);

  if( ipaddr.host==INADDR_NONE )
  {
    SJC_Write("Error: Could not resolve host");
    return -1;
  }

  pkt->address = ipaddr;
  int nchars = snprintf((char *)pkt->data, PACKET_SIZE, "%s", PROTONAME "/" VERSION);
  pkt->len = nchars;

  if( !SDLNet_UDP_Send(sock, -1, pkt) )
  {
    SJC_Write("Error: Could not send connect packet");
    SJC_Write("%s", SDL_GetError());
    net_stop();
  }

  SJC_Write("Using connex #%d", connexid);

  memset(conns + connexid, 0, sizeof *conns);
  conns[connexid].state = CONN_NEW;
  conns[connexid].addr = ipaddr;

  return 0;
}

void net_loop()
{
  int     status;
  int     i;

  if( !sock ) return;

  for( ; ; )
  {
    status = SDLNet_UDP_Recv(sock, pkt);

    if( status == -1 )
    {
      SJC_Write("Network Error: Recv failed!");
      SJC_Write(SDL_GetError());
      return;
    }

    if( status != 1 ) break;

    for( i=0; i<maxconns; i++ )
    {
      /* SJC_Write("state:%d  host:%d:%d  port:%d:%d", */
      /*     conns[i].state, conns[i].addr.host, pkt->address.host, */
      /*     conns[i].addr.port, pkt->address.port); */
      if( conns[i].state != CONN_FREE &&
          conns[i].addr.host == pkt->address.host &&
          conns[i].addr.port == pkt->address.port )
        break;
    }

    if( i == maxconns ) //a new connexion approaches
      welcome();
    else if( conns[i].state == CONN_NEW )
      acknowlege(i);
    else
      accept_from(i);
  }

  // TODO: resend packets or something?
}

int net_write(int connexid, Uint8 *data, size_t len)
{
  CONNEX_t *conn = conns + connexid;

  if( !conn || conn->state != CONN_CONNECTED )
    return -1;

  int parts = (len - 1) / PAYLOAD_SIZE + 1;
  int i;

  create_ring(conn->ringout + conn->outconga % RING_SIZE, parts);

  for( i=0; i<parts; i++ )
  {
    size_t partlen = (i == parts-1) ? (len % PAYLOAD_SIZE) : PAYLOAD_SIZE;
    create_part(connexid, i, parts, data + i*PAYLOAD_SIZE, partlen);
  }

  conn->outconga++;

  return 0;
}

Uint8 *net_read(int connexid)
{
  int i;
  size_t sum = 0;
  CONNEX_t *conn = conns + connexid;

  if( !conn )
    return NULL;

  RING_t *r = conn->ringin + conn->inconga + 1;

  for( i=0; i<r->count; i++ )
  {
    if( !r->pkt[i] )
      return NULL; // still is missing packet(s)

    sum += r->pkt[i]->len;
  }

  Uint8 *output = malloc(sum+1);

  for( i=0; i<r->count; i++ )
    memcpy(output, r->pkt[i], r->pkt[i]->len);

  output[sum] = '\0'; // safe/sorry

  // delete
  create_ring(conn->ringin, 0);
  conn->inconga++;

  return output;
}

int get_free_conn()
{
  int i;
  for( i=0; i<maxconns; i++ )
    if( conns[i].state == CONN_FREE )
      return i;
  return -1;
}

void create_ring(RING_t *r, int count)
{
  int i;

  for( i=0; i<r->count; i++ )
    if( r->pkt[i] )
      SDLNet_FreePacket(r->pkt[i]);

  r->count = count;
  r->pkt = realloc(r->pkt, sizeof *r->pkt * count);

  memset(r->pkt, 0, sizeof *r->pkt);
}

void create_part(int connexid, int partid, int count, Uint8 *data, size_t datalen)
{
  CONNEX_t *conn = conns + connexid;
  int outconga = conn->outconga;
  int inconga  = conn->inconga;
  int len = datalen + HEADER_SIZE;
  size_t n = 0;

  RING_t *r = conn->ringout + conn->outconga % RING_SIZE;
  r->count = partid;

  UDPpacket *pkt = r->pkt[partid] = SDLNet_AllocPacket(len);
  pkt->len = len;
  pkt->address = conn->addr;

  packbytes(pkt->data, outconga, &n, 4);
  pack(count, 2);
  pack(partid, 2);
  pack(inconga, 4);
  memcpy(pkt->data + n, data, datalen);

  if( !SDLNet_UDP_Send(sock, -1, pkt) )
  {
    SJC_Write("Error: Could not send new ring packet");
    SJC_Write("%s", SDL_GetError());
  }
}

void welcome()
{
  char *p = (char *)pkt->data;
  char *reply = "OK";

  if( strncmp(p, PROTONAME "/", strlen(PROTONAME) + 1) )
  {
    SJC_Write("Junk packet from %u:%u", pkt->address.host, pkt->address.port);
    reply = "Ejunk";
    goto errout;
  }

  p += strlen(PROTONAME) + 1;

  int versiondiff = strncmp(p, VERSION, strlen(VERSION));
  int lenmismatch = (pkt->len != strlen(PROTONAME) + 1 + strlen(VERSION));

  if( lenmismatch || versiondiff )
  {
    SJC_Write("Wrong protocol version \"%s\" from %u:%u",
        p, pkt->address.host, pkt->address.port);
    reply = "Eversion";
    goto errout;
  }

  int connexid = get_free_conn();

  if( connexid < 0 )
  {
    SJC_Write("New client at %u:%u denied; server is full",
        pkt->address.host, pkt->address.port);
    reply = "Efull";
    goto errout;
  }

  SJC_Write("New client at %u:%u accepted as connex #%d",
      pkt->address.host, pkt->address.port, connexid);

  memset(conns + connexid, 0, sizeof *conns);
  conns[connexid].state = CONN_CONNECTED;
  conns[connexid].addr = pkt->address;

  errout:
  snprintf((char *)pkt->data, PACKET_SIZE, "%s", reply);
  pkt->len = strlen(reply);

  if( !SDLNet_UDP_Send(sock, -1, pkt) )
  {
    SJC_Write("Error: Could not send reply packet");
    SJC_Write("%s", SDL_GetError());
  }
}

void acknowlege(int connexid)
{
  CONNEX_t *conn = conns + connexid;

  if( !pkt->len || pkt->len >= PACKET_SIZE )
    return;

  pkt->data[ pkt->len ] = '\0';

  if( pkt->data[0] == 'E' )
  {
    SJC_Write("Connection failed because: %s", pkt->data + 1);
    return;
  }
  else if( pkt->data[0] == 'O' && pkt->data[1] == 'K' )
  {
    SJC_Write("Connection successful");
    conn->state = CONN_CONNECTED;
    return;
  }

  SJC_Write("Got weird packet in acknowlege: %s", pkt->data);
}

void accept_from(int connexid)
{
  size_t n = 0;
  int conga     = unpackbytes(pkt->data, pkt->len, &n, 4);
  int count     = unpack(2);
  int partid    = unpack(2);
  int peerconga = unpack(4); // TODO: use this!

  SJC_Write("Message: %.10s", pkt->data + n);
  SJC_Write("peerconga: %d", peerconga);

  CONNEX_t *conn = conns + connexid;

  if( conga > conn->inconga + RING_SIZE - 1 )
    return; // not ready for such a future packet

  if( conga <= conn->inconga )
    return; // this is way too late

  RING_t *r = conn->ringin + conga % RING_SIZE;

  if( !r->count )
    create_ring(r, count);

  if( r->pkt[partid]->len )
    return; // already received this packet

  r->pkt[partid] = SDLNet_AllocPacket(pkt->len); // TODO: this is dumb
  r->pkt[partid]->maxlen = pkt->maxlen;
  r->pkt[partid]->len    = pkt->len;
  memcpy(r->pkt[partid]->data, pkt->data, pkt->len);
}
