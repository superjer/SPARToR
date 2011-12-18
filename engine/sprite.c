/**
 **  SPARToR
 **  Network Game Engine
 **  Copyright (C) 2010-2011  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/


#include <GL/glew.h>
#include "SDL.h"
#include "SDL_net.h"
#include "mod.h"
#include "video.h"
#include "main.h"
#include "console.h"
#include "sprite.h"


SPRITE_T *sprites;
size_t    spr_count;


enum spr_pos { TOP=-1, MID=-2, BOT=-3 };


static size_t spr_alloc;
static char   filename[100];
static int    line_num;
static FILE  *f;

#define SPR_MAX_TOKENS 100
static char *tokens[SPR_MAX_TOKENS];


static SPRITE_T *new_sprite(int texnum,const char *name);
static int fail(const char *msg);
static int tokenize(char *s);
static int read_anchor(int i,SPRITE_T *arg);


int load_sprites(int texnum)
{

  if( strlen(textures[texnum].filename) > 95 ) {
    SJC_Write("load_sprites: filename too long: %s", textures[texnum].filename);
    return -1;
  }

  sprintf(filename, "%s.txt", textures[texnum].filename);
  f = fopen(filename, "r");

  if( !f ) { return -1; }

  char line[1000];
  enum { READY, DEFAULT, GRID, GRIDITEM, NOMORE } mode = READY;
  int gridcols = 1; // just to avoid compiler warning
  int gridoffs = 0; // "
  SPRITE_T defs = {0, NULL, {0, 0, 32, 32}, 16, 32};
  SPRITE_T gdefs;

  line_num = 0;

  while( 1 ) {
    if( mode==DEFAULT || mode==NOMORE )
      mode = READY;

    if( mode==GRID )
      mode = GRIDITEM;

    line_num++;
    memset(line, 0, 1000);
    if( !fgets(line,1000,f) )
      break;

    if( line[998] )
      return fail("load_sprites: Line too long");

    int count = tokenize(line);
    int i = 0;
    SPRITE_T *spr = NULL;
    SPRITE_T *targ = NULL;

    if( count < 1 )
      continue;

    // read the first token, which should be a command or identifier
    if( tokens[i][0] != '.' ) { // sprite name found
      spr = new_sprite(texnum,tokens[i]);

      defs.texnum = gdefs.texnum = spr->texnum; // kinda lame hack so we can memcpy
      defs.name   = gdefs.name   = spr->name;

      memcpy( spr, &defs, sizeof defs );
      if( mode == GRIDITEM ) {
        memcpy( spr, &gdefs, sizeof gdefs );
        spr->rec.x += gdefs.rec.w * (gridoffs % gridcols);
        spr->rec.y += gdefs.rec.h * (gridoffs / gridcols);
        gridoffs++;
      }
      targ = spr;
    } else if( !strcmp(tokens[i],".default") ) {
      mode = DEFAULT;
      targ = &defs;
    } else if( !strcmp(tokens[i],".grid") ) {
      if( mode != READY )
        return fail("load_sprites: Not ready for a .grid command");
      mode = GRID;
      memcpy( &gdefs, &defs, sizeof defs );
      gridoffs = 0;
      targ = &gdefs;
    } else if( !strcmp(tokens[i],".end") ) {
      if( mode != GRIDITEM )
        return fail("load_sprites: .end command encountered without a .grid");
      if( count > 1 )
        return fail("unexpected tokens after .end command");
      mode = NOMORE;
    } else if( !strcmp(tokens[i],".") ) {
      if( count > 2 )
        return fail("load_sprites too many tokens after . command");
      i++;
      gridoffs += (count == 2 ? atoi(tokens[i]) : 1);
    }

    while( ++i < count ) {
      if( isdigit(tokens[i][0]) ) {
        if( count-i != 2 && count-i != 4 && count-i != 6 )
          return fail("Expecting 2, 4 or 6 numeric args, when no name found");

        targ->rec.x = atoi(tokens[  i]);
        targ->rec.y = atoi(tokens[++i]);  if( i>=count-1 ) break;
        targ->rec.w = atoi(tokens[++i]);
        targ->rec.h = atoi(tokens[++i]);  if( i>=count-1 ) break;

        i = read_anchor(i+1,targ);

      } else if( !strcmp(tokens[i],"pos") ) {
        if( count-i < 3 )
          return fail("Expecting 2 args for 'pos'");

        targ->rec.x = atoi(tokens[++i]);
        targ->rec.y = atoi(tokens[++i]);

      } else if( !strcmp(tokens[i],"size") ) {
        if( count-i < 3 )
          return fail("Expecting 2 args for 'size'");

        targ->rec.w = atoi(tokens[++i]);
        targ->rec.h = atoi(tokens[++i]);

      } else if( !strcmp(tokens[i],"anchor") ) {
        if( count-i < 3 )
          return fail("Expecting 2 args for 'anchor'");

        i = read_anchor(i+1,targ);

      } else if( !strcmp(tokens[i],"cols") ) {
        if( count-i < 2 )
          return fail("Expecting 1 arg for 'cols'");

        gridcols    = atoi(tokens[++i]);

      } else {
        SJC_Write("tokens %d: %s",i,tokens[i]);
        return fail("Unknown property name");

      }
    }

    if( spr ) {
      if(      spr->ancx == TOP ) spr->ancx = 0;
      else if( spr->ancx == MID ) spr->ancx = spr->rec.w / 2;
      else if( spr->ancx == BOT ) spr->ancx = spr->rec.w;

      if(      spr->ancy == TOP ) spr->ancy = 0;
      else if( spr->ancy == MID ) spr->ancy = spr->rec.h / 2;
      else if( spr->ancy == BOT ) spr->ancy = spr->rec.h;
    }
  }

  fclose(f);

  return 0;
}

void unload_sprites()
{
}

static SPRITE_T *new_sprite(int texnum,const char *name)
{
  if( spr_count == spr_alloc ) {
    size_t new_alloc = spr_alloc < 8 ? 8 : spr_alloc*2;
    sprites = realloc( sprites, new_alloc * sizeof *sprites );
    memset( sprites + spr_alloc, 0, (new_alloc - spr_alloc) * sizeof *sprites );
    spr_alloc = new_alloc;
  }

  sprites[spr_count].texnum = texnum;
  sprites[spr_count].name = malloc(strlen(name)+1);
  strcpy( sprites[spr_count].name, name );
  return sprites + spr_count++;
}

static int fail(const char *msg)
{
  SJC_Write("%s(%d) %s",filename,line_num,msg);
  fclose(f);
  return -1;
}

static int tokenize(char *s)
{
  int i = 0;
  int afterspace = 1;

  while( *s && *s!='#' && i<SPR_MAX_TOKENS ) {
    if( isspace(*s) ) {
      *s = '\0';
      afterspace = 1;
      s++;
      continue;
    }

    if( afterspace )
      tokens[i++] = s;

    afterspace = 0;
    s++;
  }

  *s = '\0';
  return i;
}

static int read_anchor(int i,SPRITE_T *targ)
{
  int flipme = 0;

  if(       isdigit(tokens[i][0])     ) { targ->ancx = atoi(tokens[i]); }
  else if( !strncmp(tokens[i],"to",2) ) { targ->ancx = TOP; flipme = 1; }
  else if( !strncmp(tokens[i],"le",2) ) { targ->ancx = TOP;             }
  else if( !strncmp(tokens[i],"mi",2) ) { targ->ancx = MID;             }
  else if( !strncmp(tokens[i],"bo",2) ) { targ->ancx = BOT; flipme = 1; }
  else if( !strncmp(tokens[i],"ri",2) ) { targ->ancx = BOT;             }

  ++i;
  if(       isdigit(tokens[i][0])     ) { targ->ancy = atoi(tokens[i]); }
  else if( !strncmp(tokens[i],"to",2) ) { targ->ancy = TOP;             }
  else if( !strncmp(tokens[i],"le",2) ) { targ->ancy = TOP; flipme = 1; }
  else if( !strncmp(tokens[i],"mi",2) ) { targ->ancy = MID;             }
  else if( !strncmp(tokens[i],"bo",2) ) { targ->ancy = BOT;             }
  else if( !strncmp(tokens[i],"ri",2) ) { targ->ancy = BOT; flipme = 1; }
  
  if( flipme ) SWAP( targ->ancx, targ->ancy, int );

  return i;
}

