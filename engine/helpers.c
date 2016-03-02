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

#include <stdlib.h>
#include <string.h>
#include "main.h"

#define POINTLIS_CHUNK 80

typedef void *thing;

// build a unique list of pointers with no storage limit and no storage overhead
// returns true if adding for the 1st time
int pointlis_add( thing **list, thing item )
{
        if( !list[0] ) list[0] = calloc( POINTLIS_CHUNK, sizeof **list );

        size_t n = 0;
        while( list[0][n] )
        {
                if( list[0][n] == item ) return 0; // found!
                n++;
        }

        if( n % POINTLIS_CHUNK == POINTLIS_CHUNK-1 ) // need to grow
        {
                list[0] = realloc( list[0], (n+1+POINTLIS_CHUNK) * sizeof **list );
                memset( list[0]+n+1, 0, POINTLIS_CHUNK * sizeof **list );
        }

        list[0][n] = item;
        return 1;
}

char *tok_( char **restrict buf, const char *restrict sep )
{
        if( !*buf || !**buf ) return NULL;

        char *p = *buf;
        char *found = strpbrk(p, sep);

        if( found )
                *buf = found;
        else
                while( *(*buf)++ ) ; // no body

        size_t seplen = strspn(*buf, sep);
        **buf = '\0';
        *buf += seplen;

        return p;
}

// define mkobject helper functions
#define EXPOSE(T,N,A)
#define HIDE(X)
#define STRUCT()                                                                  \
void * TOKEN_PASTE(mk,TYPE) (int *slot, int context, int frame, int flags)        \
{                                                                                 \
        int sentinel_slot;                                                        \
        if( !slot ) slot = &sentinel_slot;                                        \
                                                                                  \
        *slot = findfreeslot(frame);                                              \
        if( *slot < 0 ) return NULL;                                              \
        fr[frame].objs[*slot].type = TOKEN_PASTE(TYPE,_type);                     \
        fr[frame].objs[*slot].flags = flags;                                      \
        fr[frame].objs[*slot].context = context;                                  \
        fr[frame].objs[*slot].size = sizeof(TYPE);                                \
        return fr[frame].objs[*slot].data = calloc(1, sizeof(TYPE));              \
}
#define ENDSTRUCT(TYPE)
#include "engine_structs.h"
#include "game_structs.h"
#undef EXPOSE
#undef HIDE
#undef STRUCT
#undef ENDSTRUCT
