#ifndef SPARTOR_NET_H_
#define SPARTOR_NET_H_

#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"
#include "SDL_net.h"

#define PROTONAME "SPARToR Protocol"
#define HOSTPORT 31103
#define CLIENTPORT 31109
#define PACKET_SIZE 512
#define HEADER_SIZE 15
//#define PAYLOAD_SIZE (PACKET_SIZE - HEADER_SIZE)
#define PAYLOAD_SIZE 10
#define RING_SIZE 6

typedef struct {
  int         count;            // number of packets in the ring segment
  UDPpacket **pkt;              // all the packets
} RINGSEG_t;

typedef struct {
  IPaddress addr;               // address of peer
  int       state;              // connection state, new, connected, etc.
  int       desync;             // not meaningful yet
  int       outconga;           // the next conga to send
  int       readconga;          // the next conga for the game to read
  int       inconga;            // the latest conga we have
  int       inpart;             // the latest part we have
  int       needconga;          // the earliest conga we need
  int       needpart;           // the earliest part we need
  int       peerconga;          // the latest sequential conga of ours the peer has
  int       peerpart;           // the latest sequential part of ours the peer has
  RINGSEG_t ringout[RING_SIZE]; // ring buffer outgoing
  RINGSEG_t ringin[RING_SIZE];  // ring buffer incoming
} CONNEX_t;

extern int net_retries;
extern int net_resend;

int    net_start(int port, int _maxconns);               // start up networking
void   net_stop();                                       // stop networking
int    net_connect(const char *hostname, int port);      // establish a new connection
void   net_loop();                                       // check for updates once per main loop
void   net_test();                                       // TEMPORARY
int    net_write(int connexid, Uint8 *data, size_t len); // write to one or more connections
Uint8 *net_read(int connexid);                           // read from a connection

#endif
