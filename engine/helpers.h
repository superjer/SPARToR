#ifndef SPARTOR_HELPERS_
#define SPARTOR_HELPERS_

#include "main.h"

// macro wrapper for thread-safe strtok-like function tok_
#define tok(buf,sep) tok_(&(buf),(sep))

int pointlis_add(void ***list, void *item);
char *tok_(char **restrict buf, const char *restrict sep);

// declare mkobject helper functions
#define EXPOSE(T,N,A)
#define HIDE(X)
#define STRUCT() void * TOKEN_PASTE(mk,TYPE) (int *slot, int context, int frame, int flags);
#define ENDSTRUCT(TYPE)
#include "engine_structs.h"
#include "game_structs.h"
#undef EXPOSE
#undef HIDE
#undef STRUCT
#undef ENDSTRUCT

#define getobject(ptr, t, n)                          \
        t *sw = fr[b].objs[(n)].data;                 \
        assert( fr[b].objs[(n)].type == t ## _type );

#endif
