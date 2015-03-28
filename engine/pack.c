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

#include "SDL.h"
#include "SDL_net.h"
#include "mod.h"
#include "main.h"
#include "console.h"
#include "command.h"
#include "pack.h"

Uint8 *packframe(Uint32 packfr, size_t *n)
{
  FRAME_t *pfr  = fr + packfr % maxframes;
  int      i;
  size_t   s    = 80;
  Uint8   *data = malloc(s);

  *n = 0;

  packbytes(data, maxobjs, n, 4);

  for( i=0; i<maxobjs; i++ )
  {
    while( *n+4+sizeof(size_t)+pfr->objs[i].size >= s-1 )
      data = realloc(data,(s*=2));

    pack(pfr->objs[i].type, 2);

    if( pfr->objs[i].type )
    {
      pack(pfr->objs[i].flags  , 2);
      pack(pfr->objs[i].context, 4);
      pack(pfr->objs[i].size   , 4);
      memcpy(data+*n, pfr->objs[i].data, pfr->objs[i].size);
      *n += pfr->objs[i].size;
    }
  }

  return data;
}

Uint8 *packframecmds(Uint32 packfr, size_t *n)
{
  FRAME_t *pfr  = fr + packfr % maxframes;
  int      i;
  size_t   s    = maxclients*6+4;
  Uint8   *data = malloc(s);

  *n = 0;

  packbytes(data, maxclients, n, 4);

  for( i=0; i<maxclients; i++ )
  {
    FCMD_t *c = pfr->cmds+i;
    pack(c->cmd    , 1);
    pack(c->mousehi, 1);
    pack(c->mousex , 1);
    pack(c->mousey , 1);
    pack(c->flags  , 2);

    if( c->flags & CMDF_DATA )
    {
      pack(c->datasz, 2);
      memcpy(data+*n, c->data, c->datasz);
      *n += c->datasz;
    }

    if( c->flags & CMDF_NEW )
      SJC_Write("%u: Packed CMDF_NEW for client %d", packfr, i);
  }

  return data;
}

int unpackframe(Uint32 packfr, Uint8 *data, size_t len)
{
  FRAME_t *pfr = fr + packfr % maxframes;
  int i;
  size_t n = 0;

  if( maxobjs != (int)unpackbytes(data, len, &n, 4) )
  {
    SJC_Write("Your maxobjs setting (%d) differs from server's!", maxobjs);
    return 1;
  }

  for( i=0; i<maxobjs; i++ )
  {
    pfr->objs[i].type = unpack(2);

    if(pfr->objs[i].type)
    {
      pfr->objs[i].flags   = unpack(2);
      pfr->objs[i].context = unpack(4);
      pfr->objs[i].size    = unpack(4);

      if( pfr->objs[i].size )
      {
        if( len<n+pfr->objs[i].size )
        {
          SJC_Write("Packed data ended early!");
          return 1;
        }

        pfr->objs[i].data  = malloc(pfr->objs[i].size); //FIXME: might already be allocated with pre-net data
        memcpy(pfr->objs[i].data, data+n, pfr->objs[i].size);
        n += pfr->objs[i].size;
        mod_recvobj( pfr->objs + i );
     }
    }
  }

  return 0;
}

int unpackframecmds(Uint32 packfr, Uint8 *data, size_t len)
{
  FRAME_t *pfr = fr + packfr % maxframes;
  int i;
  size_t n = 0;

  if( maxclients != (int)unpackbytes(data, len, &n, 4) )
  {
    SJC_Write("Your maxclients setting (%d) differs from server's! packfr=%d", maxclients, packfr);
    return -1;
  }

  for( i=0; i<maxclients; i++ )
  {
    FCMD_t *c = pfr->cmds+i;
    c->cmd     = unpack(1);
    c->mousehi = unpack(1);
    c->mousex  = unpack(1);
    c->mousey  = unpack(1);
    c->flags   = unpack(2);

    if( c->flags & CMDF_DATA ) //check for variable data
    {
      c->datasz = unpack(2);

      if( c->datasz > sizeof c->data )
      {
        SJC_Write("Treachery: datasz too large (%d) from server", c->datasz);
        break;
      }

      memcpy( c->data, data+n, c->datasz );
      n += c->datasz;
    }

    if( c->flags & CMDF_NEW )
      SJC_Write("%u: Unpacked CMDF_NEW for client %d", packfr, i);
  }

  return n;
}

void packbytes(Uint8 *_data, Uint64 value, size_t *_offset, int width)
{
  static Uint8  *data;
  static size_t *offset;
  size_t         n = 0;

  if( _data )
  {
    data   = _data;
    offset = _offset;
  }

  if( !offset ) offset = &n;

  switch( width )
  {
    case 8: *(data+(*offset)++) = (Uint8)(value>>56);
    case 7: *(data+(*offset)++) = (Uint8)(value>>48);
    case 6: *(data+(*offset)++) = (Uint8)(value>>40);
    case 5: *(data+(*offset)++) = (Uint8)(value>>32);
    case 4: *(data+(*offset)++) = (Uint8)(value>>24);
    case 3: *(data+(*offset)++) = (Uint8)(value>>16);
    case 2: *(data+(*offset)++) = (Uint8)(value>>8 );
    case 1: *(data+(*offset)++) = (Uint8)(value    );
  }
}

Uint64 unpackbytes(Uint8 *_data, size_t _len, size_t *_offset, int width)
{
  static Uint8  *data;
  static size_t  len;
  static size_t *offset;
  Uint64         value = 0;
  size_t         n = 0;

  // new buffer incoming
  if( _data )
  {
    data   = _data;
    len    = _len;
    offset = _offset;
  }

  if( !offset ) offset = &n;

  if( len<*offset+width )
  {
    SJC_Write("Not enough packed bytes to read!");
    return 0;
  }

  switch( width )
  {
    case 8: value |= ((Uint64)*(data+(*offset)++))<<56;
    case 7: value |= ((Uint64)*(data+(*offset)++))<<48;
    case 6: value |= ((Uint64)*(data+(*offset)++))<<40;
    case 5: value |= ((Uint64)*(data+(*offset)++))<<32;
    case 4: value |= ((Uint64)*(data+(*offset)++))<<24;
    case 3: value |= ((Uint64)*(data+(*offset)++))<<16;
    case 2: value |= ((Uint64)*(data+(*offset)++))<<8 ;
    case 1: value |= ((Uint64)*(data+(*offset)++))    ;
  }

  return value;
}

void inspectbytes( Uint8 *data, int n )
{
  int i = 0;
  for( ; i<n; i++ )
    SJC_Write("Byte %d: %d",i,data[i]);
}
