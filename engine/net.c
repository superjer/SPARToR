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
#include "net.h"


Uint8 *packframe(Uint32 packfr,size_t *n) {
  FRAME_t *pfr = fr + packfr % maxframes;
  int i;
  size_t s = 80;
  Uint8 *data = malloc(s);
  *n = 0;

  packbytes(data,maxobjs,n,4);
  for(i=0;i<maxobjs;i++) {
    while( *n+4+sizeof(size_t)+pfr->objs[i].size >= s-1 )
      data = realloc(data,(s*=2));
    packbytes(data,pfr->objs[i].type,n,2);
    if(pfr->objs[i].type) {
      packbytes(data,pfr->objs[i].flags,n,2);
      packbytes(data,pfr->objs[i].context,n,4);
      packbytes(data,pfr->objs[i].size ,n,sizeof(size_t));
      memcpy(data+*n, pfr->objs[i].data, pfr->objs[i].size);
      *n += pfr->objs[i].size;
    }
  }
  return data;
}


Uint8 *packframecmds(Uint32 packfr,size_t *n) {
  FRAME_t *pfr = fr + packfr % maxframes;
  int i;
  size_t s = maxclients*6+4;
  Uint8 *data = malloc(s);
  *n = 0;

  packbytes(data,maxclients,n,4);
  for(i=0;i<maxclients;i++) {
    FCMD_t *c = pfr->cmds+i;
    packbytes(data,c->cmd    ,n,1);
    packbytes(data,c->mousehi,n,1);
    packbytes(data,c->mousex ,n,1);
    packbytes(data,c->mousey ,n,1);
    packbytes(data,c->flags  ,n,2);
    if( c->flags & CMDF_DATA ) {
      packbytes(data,c->datasz     ,n,2);
      memcpy(data+*n,c->data,c->datasz);
      *n += c->datasz;
    }
    if( c->flags & CMDF_NEW )
      SJC_Write("%u: Packed CMDF_NEW for client %d",packfr,i);
  }
  return data;
}


int unpackframe(Uint32 packfr,Uint8 *data,size_t len) {
  FRAME_t *pfr = fr + packfr % maxframes;
  int i;
  size_t n = 0;

  if( maxobjs!=(int)unpackbytes(data,len,&n,4) ) {
    SJC_Write("Your maxobjs setting (%d) differs from server's!",maxobjs);
    return 1;
  }
  for(i=0;i<maxobjs;i++) {
    pfr->objs[i].type = unpackbytes(data,len,&n,2);
    if(pfr->objs[i].type) {
      pfr->objs[i].flags   = unpackbytes(data,len,&n,2);
      pfr->objs[i].context = unpackbytes(data,len,&n,4);
      pfr->objs[i].size    = unpackbytes(data,len,&n,sizeof(size_t));
      if( pfr->objs[i].size ) {
        if( len<n+pfr->objs[i].size ) {
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


int unpackframecmds(Uint32 packfr,Uint8 *data,size_t len) {
  FRAME_t *pfr = fr + packfr % maxframes;
  int i;
  size_t n = 0;

  if( maxclients!=(int)unpackbytes(data,len,&n,4) ) {
    SJC_Write("Your maxclients setting (%d) differs from server's! packfr=%d",maxclients,packfr);
    return -1;
  }
  for(i=0;i<maxclients;i++) {
    FCMD_t *c = pfr->cmds+i;
    c->cmd     = unpackbytes(data,len,&n,1);
    c->mousehi = unpackbytes(data,len,&n,1);
    c->mousex  = unpackbytes(data,len,&n,1);
    c->mousey  = unpackbytes(data,len,&n,1);
    c->flags   = unpackbytes(data,len,&n,2);
    if( c->flags & CMDF_DATA ) { //check for variable data
      c->datasz = unpackbytes(data,len,&n,2);
      if( c->datasz > sizeof c->data ) {
        SJC_Write("Treachery: datasz too large (%d) from server",c->datasz);
        break;
      }
      memcpy( c->data, data+n, c->datasz );
      n += c->datasz;
    }
    if( c->flags & CMDF_NEW )
      SJC_Write("%u: Unpacked CMDF_NEW for client %d",packfr,i);
  }
  return n;
}


void packbytes(Uint8 *data,Uint64 value,size_t *offset,int width) {
  size_t n = 0;
  if( offset==NULL ) offset = &n;
  switch(width) {
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


Uint64 unpackbytes(Uint8 *data,size_t len,size_t *offset,int width) {
  Uint64 value = 0;
  size_t n = 0;
  if( offset==NULL ) offset = &n;
  if( len<*offset+width ) {
    SJC_Write("Not enough packed bytes to read!");
    return 0;
  }
  switch(width) {
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


void inspectbytes(Uint8 *data,int n) {
  int i = 0;
  for(;i<n;i++)
    SJC_Write("Byte %d: %d",i,data[i]);
}

