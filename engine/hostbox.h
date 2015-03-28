#ifndef SPARTOR_HOSTBOX_H_
#define SPARTOR_HOSTBOX_H_

#include "SDL.h"
#include "SDL_net.h"

void host_send_state(int clientid);
void host_outbox_write();
void host_inbox_read(int clientid, CLIENT_t *cl);

#endif
