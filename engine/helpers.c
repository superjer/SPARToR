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


#include <stdlib.h>
#include <string.h>


#define POINTLIS_CHUNK 80


typedef void *thing;


// build a unique list of pointers with no storage limit and no storage overhead
// returns true if adding for the 1st time
int pointlis_add( thing **list, thing item )
{
  if( !list[0] ) list[0] = calloc( POINTLIS_CHUNK, sizeof **list );

  size_t n = 0;
  while( list[0][n] ) {
    if( list[0][n] == item ) return 0; // found!
    n++;
  }

  if( n % POINTLIS_CHUNK == POINTLIS_CHUNK-1 ) { // need to grow
    list[0] = realloc( list[0], (n+1+POINTLIS_CHUNK) * sizeof **list );
    memset( list[0]+n+1, 0, POINTLIS_CHUNK * sizeof **list );
  }

  list[0][n] = item;
  return 1;
}
