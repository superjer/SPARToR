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
#include "main.h"
#include "net.h"
#include "pack.h"
#include "console.h"

enum CONN_STATES {
  CONN_FREE = 0,
  CONN_NEW,
  CONN_CONNECTED,
};

int net_retries = 0;

static UDPsocket  sock = NULL;
static UDPpacket *pkt;
static UDPpacket *initpkt;
static CONNEX_t  *conns;
static int        maxconns;

static int get_free_conn();
static void send_connect();
static void create_ring(RINGSEG_t *r, int count);
static void create_part(int connexid, int partid, int count, Uint8 *data, size_t len);
static void welcome();
static void acknowlege(int connexid);
static void accept_from(int connexid);
static void unlose_packet(int connexid);
static void reevaluate_inconga(int connexid);

int net_start(int port, int _maxconns)
{
  if( sock )
  {
    echo("Error: Socket already open");
    return -1;
  }

  net_retries = 0;
  sock = SDLNet_UDP_Open(port);

  if( !sock )
  {
    echo("Unable to open port %d", port);
    echo("%s", SDL_GetError());
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
  pkt = NULL;
}

int net_connect(const char *hostname, int port)
{
  IPaddress ipaddr;
  int connexid;

  if( (connexid = get_free_conn()) < 0 )
  {
    echo("Error: No free connection");
    return -1;
  }

  SDLNet_ResolveHost(&ipaddr, hostname, port);

  if( ipaddr.host==INADDR_NONE )
  {
    echo("Error: Could not resolve host");
    return -1;
  }

  initpkt = SDLNet_AllocPacket(PACKET_SIZE);
  initpkt->address = ipaddr;
  int nchars = snprintf((char *)initpkt->data, PACKET_SIZE, "%s", PROTONAME "/" VERSION);
  initpkt->len = nchars;

  net_retries = 1;

  echo("Using connex #%d", connexid);

  memset(conns + connexid, 0, sizeof *conns);
  conns[connexid].state = CONN_NEW;
  conns[connexid].addr = ipaddr;

  return 0;
}

void net_loop()
{
  int           status;
  int           i;
  static Uint32 prevticks = 0;

  if( !sock ) return;

  if( !prevticks ) prevticks = ticks;

  Uint32 tickdiff = ticks - prevticks;

  if( net_retries )
  {
    net_retries += tickdiff;
    if( net_retries > 2000 )
    {
      echo("Retrying to connect...");
      send_connect();
      net_retries = 1;
    }
  }

  prevticks = ticks;

  for( ;; )
  {
    status = SDLNet_UDP_Recv(sock, pkt);

    if( status == -1 )
    {
      echo("Network Error: Recv failed!");
      echo(SDL_GetError());
      return;
    }

    if( status != 1 ) break;

    for( i=0; i<maxconns; i++ )
    {
      /* echo("state:%d  host:%d:%d  port:%d:%d", */
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

  return;
  static Uint32 ticksoup = 0;
  ticksoup += tickdiff;

  // resend packets or something?
  if( ticksoup > 5000 )
  {
    ticksoup = 0;
    for( i=0; i<maxconns; i++ )
      if( conns[i].state == CONN_CONNECTED ) //TODO something on other states?
        unlose_packet(i);
  }
}

void net_test()
{
  int i;

  for( i=0; i<maxconns; i++ )
  {
    Uint8 *p = net_read(i);
    if( p )
    {
      echo("net_test message: \"%s\"", p);
      free(p);
    }
  }
}

int net_write(int connexid, Uint8 *data, size_t len)
{
  CONNEX_t *conn = conns + connexid;

  if( !conn || conn->state != CONN_CONNECTED )
    return -1;

  int parts = (len-1) / PAYLOAD_SIZE + 1;
  int i;

  conn->outconga++;
  create_ring(conn->ringout + conn->outconga % RING_SIZE, parts);

  for( i=0; i<parts; i++ )
  {
    size_t partlen = (i == parts-1) ? ((len-1) % PAYLOAD_SIZE + 1) : PAYLOAD_SIZE;
    create_part(connexid, i, parts, data + i*PAYLOAD_SIZE, partlen);
  }

  return 0;
}

Uint8 *net_read(int connexid)
{
  int i;
  size_t sum = 0;
  CONNEX_t *conn = conns + connexid;

  if( !conn )
    return NULL;

  RINGSEG_t *r = conn->ringin + (conn->readconga + 1) % RING_SIZE;

  if( !r->count )
    return NULL;

  /* echo("net_read: readconga %d, count %d", conn->readconga, r->count); */

  for( i=0; i<r->count; i++ )
  {
    if( !r->pkt[i] )
      return NULL; // still is missing packet(s)

    sum += r->pkt[i]->len - HEADER_SIZE;
  }

  Uint8 *output = malloc(sum+1);
  Uint8 *p = output;

  for( i=0; i<r->count; i++ )
  {
    size_t len = r->pkt[i]->len - HEADER_SIZE;
    memcpy(p, r->pkt[i]->data + HEADER_SIZE, len);
    p += len;
  }

  *p = '\0'; // safe/sorry

  // delete
  create_ring(r, 0);
  conn->readconga++;

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

void send_connect()
{
  if( SDLNet_UDP_Send(sock, -1, initpkt) )
    return;

  echo("Error: Could not send connect packet");
  echo("%s", SDL_GetError());
  net_stop();
}

void create_ring(RINGSEG_t *r, int count)
{
  int i;

  for( i=0; i<r->count; i++ )
    if( r->pkt[i] )
      SDLNet_FreePacket(r->pkt[i]);

  r->count = count;
  r->pkt = realloc(r->pkt, sizeof *r->pkt * count);

  if( count )
    memset(r->pkt, 0, sizeof *r->pkt * count);
}

void create_part(int connexid, int partid, int count, Uint8 *data, size_t datalen)
{
  CONNEX_t *conn = conns + connexid;
  int len = datalen + HEADER_SIZE;
  size_t n = 0;

  RINGSEG_t *r = conn->ringout + conn->outconga % RING_SIZE;
  r->count = partid;

  UDPpacket *pkt = r->pkt[partid] = SDLNet_AllocPacket(len);
  pkt->len = len;
  pkt->address = conn->addr;

  packbytes(pkt->data, 'x', &n, 1);
  pack(conn->outconga, 4);
  pack(count, 2);
  pack(partid, 2);
  pack(conn->inconga, 4);
  pack(conn->inpart, 2);
  assert(HEADER_SIZE == n);
  memcpy(pkt->data + n, data, datalen);

#if 0 //packet loss?!
  static int sendsies = 0;
  if( sendsies++ == 1 )
  {
    echo("JE REFUSE!");
    return;
  }
#endif

  if( !SDLNet_UDP_Send(sock, -1, pkt) )
  {
    echo("Error: Could not send new ring packet");
    echo("%s", SDL_GetError());
  }
}

void welcome()
{
  char *p = (char *)pkt->data;
  char *reply = "OK";

  if( strncmp(p, PROTONAME "/", strlen(PROTONAME) + 1) )
  {
    echo("Junk packet from %u:%u", pkt->address.host, pkt->address.port);
    reply = "Ejunk";
    goto errout;
  }

  p += strlen(PROTONAME) + 1;

  int versiondiff = strncmp(p, VERSION, strlen(VERSION));
  int lenmismatch = (pkt->len != strlen(PROTONAME) + 1 + strlen(VERSION));

  if( lenmismatch || versiondiff )
  {
    echo("Wrong protocol version \"%s\" from %u:%u",
        p, pkt->address.host, pkt->address.port);
    reply = "Eversion";
    goto errout;
  }

  int connexid = get_free_conn();

  if( connexid < 0 )
  {
    echo("New client at %u:%u denied; server is full",
        pkt->address.host, pkt->address.port);
    reply = "Efull";
    goto errout;
  }

  echo("New client at %u:%u accepted as connex #%d",
      pkt->address.host, pkt->address.port, connexid);

  memset(conns + connexid, 0, sizeof *conns);
  conns[connexid].state = CONN_CONNECTED;
  conns[connexid].addr = pkt->address;

  errout:
  snprintf((char *)pkt->data, PACKET_SIZE, "%s", reply);
  pkt->len = strlen(reply);

  if( !SDLNet_UDP_Send(sock, -1, pkt) )
  {
    echo("Error: Could not send reply packet");
    echo("%s", SDL_GetError());
  }
}

void acknowlege(int connexid)
{
  CONNEX_t *conn = conns + connexid;

  if( initpkt )
  {
    SDLNet_FreePacket(initpkt);
    initpkt = NULL;
    net_retries = 0;
  }

  if( !pkt->len || pkt->len >= PACKET_SIZE )
    return;

  pkt->data[ pkt->len ] = '\0';

  if( pkt->data[0] == 'E' )
  {
    echo("Connection failed because: %s", pkt->data + 1);
    return;
  }
  else if( pkt->data[0] == 'O' && pkt->data[1] == 'K' )
  {
    echo("Connection successful");
    conn->state = CONN_CONNECTED;
    return;
  }

  echo("Got weird packet in acknowlege: %s", pkt->data);
}

void accept_from(int connexid)
{
  size_t n = 0;
  char ident    = unpackbytes(pkt->data, pkt->len, &n, 1);
  int conga     = unpack(4);
  int count     = unpack(2);
  int partid    = unpack(2);
  int peerconga = unpack(4);
  int peerpart  = unpack(2);
  assert(HEADER_SIZE == n);

  CONNEX_t *conn = conns + connexid;

  if( ident != 'x' )
  {
    echo("Wrong packet format - ignored");
    return;
  }

  echo("%s: \\#74F%d.%d/%d \\#FFFpeer \\#F92%d.%d",
      __func__, conga, partid, count, peerconga, peerpart);

  // update our idea of where the peer is
  if( conn->peerconga < peerconga &&
      (conn->peerconga == peerconga || conn->peerpart < peerpart) )
  {
    conn->peerconga = peerconga;
    conn->peerpart = peerpart;
  }

  if( conga > conn->readconga + RING_SIZE - 1 )
    return; // not ready for such a future packet

  if( conga <= conn->readconga )
    return; // this is way too late

  RINGSEG_t *r = conn->ringin + conga % RING_SIZE;

  /* echo("conga%d \\#F00r->count%d", conga, r->count); */
  if( !r->count )
    create_ring(r, count);
  /* echo("conga%d \\#F80r->count%d", conga, r->count); */

  if( partid >= r->count ) { echo("partid too high!"); return; }

  /* echo("accept_from: conga %d, peerconga %d, count %d, partid %d, r->pkt %p, r->pkt[partid] %p", */
  /*     conga, peerconga, count, partid, r->pkt, r->pkt[partid]); */

  if( r->pkt[partid] && r->pkt[partid]->len )
  {
    echo("accept_from: packet already received");
    return; // already received this packet
  }

  // TODO: this is dumb; we're never going to send this or anything; so does not need to be a real pkt
  r->pkt[partid] = SDLNet_AllocPacket(pkt->len);
  r->pkt[partid]->maxlen = pkt->maxlen;
  r->pkt[partid]->len    = pkt->len;
  memcpy(r->pkt[partid]->data, pkt->data, pkt->len);

  reevaluate_inconga(connexid);
}

void unlose_packet(int connexid)
{
  CONNEX_t *conn = conns + connexid;
  int peerpart = conn->peerpart;

  if( conn->peerconga > conn->outconga )
    echo("Peer claims future packet: %d, outconga is %d", conn->peerconga, conn->outconga);

  if( conn->peerconga == conn->outconga ) echo("peerconga = outconga"); ///////////////////////////

  // nothing to do if peer is caught up
  if( conn->peerconga >= conn->outconga )
    return;

  // maybe peer wants a packet lost to history?
  if( conn->peerconga < conn->outconga - RING_SIZE + 1 )
  {
    // TODO: but we *could* still be ok, right guys?
    echo("Desync");
    conn->desync = 1;
    return;
  }

  RINGSEG_t *r = conn->ringout + conn->peerconga % RING_SIZE;

  // check for bad part id
  if( peerpart < 0 || peerpart >= r->count )
  {
    echo("Peer part out of range: %d / %d -- peerconga: %d", peerpart, r->count, conn->peerconga);
    return;
  }

  peerpart++;

  // do we need to roll over to the next conga?
  if( peerpart == r->count )
  {
    r = conn->ringout + (conn->peerconga+1) % RING_SIZE;
    peerpart = 0;
  }

  // nothing to send?
  if( peerpart >= r->count )
    return;

  echo("Resending outconga %d, part %d", r - conn->ringout, peerpart);

  if( !SDLNet_UDP_Send(sock, -1, r->pkt[peerpart]) )
  {
    echo("Error: Could not send unlost packet");
    echo("%s", SDL_GetError());
  }
}

void reevaluate_inconga(int connexid)
{
  CONNEX_t *conn = conns + connexid;
  int inconga = conn->inconga;
  int inpart  = conn->inpart;

  for( ;; )
  {
    RINGSEG_t *r = conn->ringin + inconga % RING_SIZE;

    // get out when we hit an empty ring segment
    // unless we're behind readconga which indicates we're looking at a deleted ring segment
    if( conn->inconga > conn->readconga && r->count == 0 )
    {
      echo("%s: \\#F3FexitA \\#888conn->readconga%d conn->inconga%d conn->inpart%d",
          __func__, conn->readconga, conn->inconga, conn->inpart);
      return;
    }

    // move to next conga once we've seen all the parts
    if( inpart >= r->count )
    {
      inconga++;
      inpart = 0;
      continue;
    }

    // get out when we hit a missing part
    if( !r->pkt[inpart] )
    {
      echo("%s: \\#39FexitB \\#888conn->readconga%d conn->inconga%d conn->inpart%d",
          __func__, conn->readconga, conn->inconga, conn->inpart);
      return;
    }

    conn->inconga = inconga;
    conn->inpart = inpart++;
  }
}
