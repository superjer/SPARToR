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

void host_send_state(int clientid);
void host_outbox_write();
void host_inbox_read(int clientid, CLIENT_t *cl);

#endif
