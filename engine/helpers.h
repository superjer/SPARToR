#ifndef SPARTOR_HELPERS_
#define SPARTOR_HELPERS_

#include "main.h"

// macro wrapper for thread-safe strtok-like function tok_
#define tok(buf,sep) tok_(&(buf),(sep))

int pointlis_add(void ***list, void *item);
char *tok_(char **restrict buf, const char *restrict sep);

#define mkobj(t, slot, context, frame, flags)                            \
        mkobj_(sizeof(t ## _t), OBJT_ ## t, slot, context, frame, flags)

void *mkobj_(size_t sz, int type, int *slot, int context, int frame, int flags);

#define GETOBJ( ptr, t, n )                           \
        t ## _t *sw = fr[b].objs[(n)].data;           \
        assert( fr[b].objs[(n)].type == OBJT_ ## t );

#endif
