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

/**
 **  map file header format:
 **   gamename mapversion spritecount
 **   spritename
 **   spritename
 **   ...
 **   projection-mode tileUW tileUH
 **   blocksizeX blocksizeY blocksizeZ
 **   dimensionX dimensionY dimensionZ
 **   [blockdata...]
 **/

#include "mod.h"
#include "main.h"
#include "saveload.h"
#include "sprite.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


const char *create_context(CONTEXT_t *co, const CONTEXT_t *ref, int x, int y, int z);


#define B85_1(x) (b85alphabet[((x)              )%85])
#define B85_2(x) (b85alphabet[((x)/(         85))%85])
#define B85_3(x) (b85alphabet[((x)/(      85*85))%85])
#define B85_4(x) (b85alphabet[((x)/(   85*85*85))%85])
#define B85_5(x) (b85alphabet[((x)/(85*85*85*85))%85])


static char b85alphabet[] = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstu";

Uint32 from_b85( const char *s )
{
  int i;
  char *p;
  Uint32 sum = 0;

  for( i=0; i<5; i++ ) {
    if( !s[i] || !( p = strchr(b85alphabet,s[i]) ) )
      break;
    sum = sum*85 + (p-b85alphabet);
  }
  return sum;
}


char *sjtempnam(const char *dir, const char *pfx, const char *ext)
{
  FILE *f;
  char *buf = malloc(256);
  int i;

  for( i=0; i<9999; i++ ) {
    snprintf( buf, 256, "%s/%s%d%s", dir, pfx, i, ext);
    f = fopen( buf, "r" );
    if( !f )
      return buf;
    fclose( f );
  }

  return NULL;
}


int save_context(const char *name,int context,int savefr)
{
  char  path[256];
  char  bakdir[256];
  char *bakfile;

  snprintf( path,   256, GAME "/maps/%s.txt", name );
  snprintf( bakdir, 256, GAME "/maps/backup" );

  // attempt to backup existing file by moving it to backup directory
  if( !(bakfile = sjtempnam(bakdir,name,".txt")) ) {
    echo("Failed to create temporary name");
    return -1;
  }

  // fail if any error occurs other than that the file does not already exist
  if( -1 == rename(path,bakfile) ) {
    if( errno != ENOENT ) {
      echo("Failed to backup old file %s",path);
      free( bakfile );
      return -1;
    }
  } else
    echo("Old file backed up to %s",bakfile);

  free( bakfile );

  // now that the existing file, if any, is safe, open file to write to
  FILE *f = fopen( path, "w" );
  if( !f ) {
    echo("Failed to open file for writing: %s",path);
    return -1;
  }

  savefr = savefr%maxframes;
  CONTEXT_t *co = fr[savefr].objs[context].data;
  int x,y,z;

  // find textures in use
  int i;
  int in_use[ spr_count ];
  memset( in_use, -1, sizeof in_use[0] * spr_count );
  for( i=0; i<co->x*co->y*co->z; i++ ) {
    size_t n = co->dmap[i].spr;

    if( n < spr_count )
      in_use[n] = 0; // zero means used, -1 means unusued
    else
      echo("Warning: sprite #%d is in use but is out of bounds (spr_count=%d)",n,spr_count);
  }

  // get count of used sprites, and number each one sequentially in in_use[]
  int use_count = 0;
  for( i=0; i<(int)spr_count; i++ )
    if( in_use[i] != -1 )
      in_use[i] = use_count++;

  // start writing the file
  if(0>fprintf( f, "%s %i %i\n", GAMENAME, MAPVERSION, use_count )) goto fail;

  for( i=0; i<(int)spr_count; i++ )
    if( in_use[i]!=-1 && 0>fprintf( f, "%s\n", sprites[i].name ) ) goto fail;

  const char *proj = (co->projection == DIMETRIC ? "DIMETRIC" : "ORTHOGRAPHIC");
  if(0>fprintf( f, "%s %i %i\n", proj, co->tileuw, co->tileuh )) goto fail;

  if(0>fprintf( f, "%i %i %i\n", co->bsx, co->bsy, co->bsz )) goto fail;

  if(0>fprintf( f, "%i %i %i\n", co->x,   co->y,   co->z   )) goto fail;

  for( z=0; z<co->z; z++ ) {
    for( y=0; y<co->y; y++ ) {
      for( x=0; x<co->x; x++ ) {
        int pos = co->x*co->y*z + co->x*y + x;
        CB *cb = co->dmap+pos;
        size_t val = in_use[cb->spr];

        if( cb->flags ) {
          if(0>fprintf( f, " %c%c%c%c~%.2x", B85_4(val), B85_3(val), B85_2(val), B85_1(val), cb->flags )) goto fail;
        } else {
          if(0>fprintf( f, " %c%c%c%c   "  , B85_4(val), B85_3(val), B85_2(val), B85_1(val)            )) goto fail;
        }
      }
      if(0>fprintf( f, "\n" )) goto fail;
    }
    if(0>fprintf( f, "\n" )) goto fail;
  }

  fclose(f);
  echo("Current context saved to %s",path);
  return 0;

  fail:
  fclose(f);
  echo("There was an error while writing the file!");
  return -1;
}


int fail(FILE *f,const char *msg)
{
  echo("Context loading error: %s",msg);
  fclose(f);
  return -1;
}


int load_context(const char *name,int context,int loadfr)
{
  char path[256];
  int i;

  snprintf( path, 256, GAME "/maps/%s.txt", name );

  FILE *f = fopen( path, "r" );
  if( !f ) {
    echo("Failed to open file for reading: %s",path);
    return -1;
  }

  char buf[256];
  int version = 0;
  int nspr = 0;
  if( 3 != fscanf(f,"%100s %d %d\n",buf,&version,&nspr) )         return fail(f,"failed to read line 1");
  if( 0 != strcmp(buf,GAMENAME) )                                 return fail(f,"GAMENAME mismatch");
  if( version != MAPVERSION )                                     return fail(f,"MAPVERSION mismatch");
  if( nspr<1 || nspr>65535 )                                      return fail(f,"invalid sprite count");

  char sprnames[nspr][100];
  int sprnumbers[nspr];
  memset( sprnumbers, 0, nspr * sizeof *sprnumbers );
  for( i=0; i<nspr; i++ ) {
    if( 1 != fscanf(f,"%100s\n",sprnames[i]) )                    return fail(f,"error reading sprite name");
    sprnumbers[i] = find_sprite_by_name(sprnames[i]);
  }

  int proj;
  int tileuw,tileuh;
  if( 3 != fscanf(f,"%80s %d %d\n",buf,&tileuw,&tileuh) )         return fail(f,"failed to read context projection");
  if(      !strcmp(buf,"ORTHOGRAPHIC") ) proj = ORTHOGRAPHIC;
  else if( !strcmp(buf,"DIMETRIC")     ) proj = DIMETRIC;
  else                                                            return fail(f,"unknown projection mode");
  if( tileuw<1 || tileuw>1024 )                                   return fail(f,"tileuw is out of range");
  if( tileuh<1 || tileuh>1024 )                                   return fail(f,"tileuh is out of range");

  int bsx,bsy,bsz;
  if( 3 != fscanf(f,"%d %d %d\n",&bsx,&bsy,&bsz) )                return fail(f,"failed to read context size"); 
  if( bsx<1 || bsx>1024 )                                         return fail(f,"bsx is out of range");
  if( bsy<1 || bsy>1024 )                                         return fail(f,"bsy is out of range");
  if( bsz<1 || bsz>1024 )                                         return fail(f,"bsz is out of range");

  int x,y,z;
  if( 3 != fscanf(f,"%d %d %d\n",&x,&y,&z) )                      return fail(f,"failed to read dimensions");

  const char *error = NULL;
  CONTEXT_t tmp_co;

  if( (error = create_context(&tmp_co,NULL,x,y,z)) )              return fail(f,error);

  CB *map  = tmp_co.map;
  CB *dmap = tmp_co.dmap;
  int volume = x*y*z;

  for( i=0; i<volume; i++ ) {
    Uint32  flags = 0;
    int     n;
    char    b85str[6] = {0};

    if( 1 > fscanf(f," %5[^ vwxyz{|}~]~%x ",b85str,&flags) ) //   return fail(f,"failed to read block data");
      n = 0;
    else
      n = from_b85(b85str);

    if( n >= nspr ) echo("sprite number is too high! (%d/%d)",n,nspr);

    dmap[i].spr   = map[i].spr   = (n < nspr ? sprnumbers[n] : 0); // avoid overread if tex is too high
    dmap[i].flags = map[i].flags = flags;
    dmap[i].flags |= CBF_NULL;
  }

  // everything ok? swap it in
  loadfr = loadfr%maxframes;
  CONTEXT_t *co = fr[loadfr].objs[context].data;
  co->projection = proj;
  co->tileuw = tileuw;
  co->tileuh = tileuh;
  co->bsx = bsx;
  co->bsy = bsy;
  co->bsz = bsz;
  co->x = x;
  co->y = y;
  co->z = z;

  // FIXME: MEM LEAK!
  // Using resprite shortly after unloading a context WOULD cause a crash
  // Garbage collection of old contexts is NOT implemented yet, so they just leak for now
  // FIXME: MEM LEAK!

  // if( co->map  ) free(co->map ); //FIXME remove hack_map someday and don't free this stuff here
  // if( co->dmap ) free(co->dmap);

  co->map  = hack_map  = map;
  co->dmap = hack_dmap = dmap;

  fclose(f);

  return 0;
}


// allocate the map, dmap for a new context, copying data from a reference context if not NULL
const char *create_context(CONTEXT_t *co, const CONTEXT_t *ref, int x, int y, int z)
{
  if( x<1 || x>9000 || y<1 || y>9000 || z<1 || z>9000 ) {
    if( !ref ) return "No reference and bounds out-of-range!";
    x = ref->x;
    y = ref->y;
    z = ref->z;
  }

  memset( co, 0, sizeof *co );

  if( ref ) *co = *ref;

  co->x = x;
  co->y = y;
  co->z = z;

  int volume = x * y * z;

  co->map  = malloc( (sizeof *co->map ) * volume );
  co->dmap = malloc( (sizeof *co->dmap) * volume );

  int i, j, k;

  // copy block data if possible, or set to blank
  for( i=0; i<x; i++ ) for( j=0; j<y; j++ ) for( k=0; k<z; k++ ) {
    int offs_co = co->x*co->y*k + co->x*j + i;

    if( ref && i < ref->x && j < ref->y && k < ref->z ) {
      int offs_ref = ref->x*ref->y*k + ref->x*j + i;

      co->map [ offs_co ] = ref->map [ offs_ref ];
      co->dmap[ offs_co ] = ref->dmap[ offs_ref ];
    } else {
      memset( co->map  + offs_co, 0, sizeof co->map [ offs_co ] );
      memset( co->dmap + offs_co, 0, sizeof co->dmap[ offs_co ] );
    }
  }

  return NULL;
}

void destroy_context(CONTEXT_t *co, int really)
{
  if( really ) {
    free(co->map);
    free(co->dmap);
  }
  co->map = co->dmap = NULL;
}

#define UNDOLEVELS 25
static CONTEXT_t constack[UNDOLEVELS];

void push_context(CONTEXT_t *co)
{
  destroy_context( constack + UNDOLEVELS-1, 1 );
  memmove( constack+1, constack, sizeof *constack * (UNDOLEVELS-1) );
  create_context( constack, co, 0, 0, 0 );
}

void pop_context(CONTEXT_t *co)
{
  if( !constack->map ) { echo("No context to pop!"); return; }
  destroy_context( co, 1 );
  *co = *constack;
  memmove( constack, constack+1, sizeof *constack * (UNDOLEVELS-1) );
  constack[UNDOLEVELS-1].map = NULL;
}
