#ifndef SPARTOR_NET_H_
#define SPARTOR_NET_H_

#include "SDL.h"
#include "SDL_net.h"

#define PROTONAME "SPARToR Protocol"
#define HOSTPORT 31103
#define CLIENTPORT 31109
#define PACKET_SIZE 512
#define HEADER_SIZE 12
#define PAYLOAD_SIZE (PACKET_SIZE - HEADER_SIZE)
#define RING_SIZE 1000

typedef struct {
  int         count;
  UDPpacket **pkt;
} RING_t;

typedef struct {
  IPaddress addr;
  int       state;
  int       outconga;
  int       inconga;
  RING_t    ringout[RING_SIZE];
  RING_t    ringin[RING_SIZE];
} CONNEX_t;

int    net_start(int port, int _maxconns);               // start up networking
void   net_stop();                                       // stop networking
int    net_connect(const char *hostname, int port);      // establish a new connection
void   net_loop();                                       // check for updates once per main loop
int    net_write(int connexid, Uint8 *data, size_t len); // write to one or more connections
Uint8 *net_read(int connexid);                           // read from a connection

#endif
