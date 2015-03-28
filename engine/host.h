#ifndef SPARTOR_HOST_H_
#define SPARTOR_HOST_H_

#include "SDL.h"
#include "SDL_net.h"

typedef struct{
  IPaddress addr;
  int connected;
  int lastconga;
  size_t buflen;
  size_t maxlen;
  Uint8 *buf;
} CLIENT_t;

extern UDPsocket  hostsock;

void host_write_packets();
void host_read_packet(int clientid);

#endif
