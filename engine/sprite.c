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


#include <GL/glew.h>
#include "SDL.h"
#include "SDL_net.h"
#include "mod.h"
#include "video.h"
#include "main.h"
#include "console.h"
#include "sprite.h"


SPRITE_T  *sprites;
size_t     spr_count;
char      *spr_names[] = { SPRITE_ENUM(STRINGIFY) };
int        spr_map[sprite_enum_max] = { 0 };

SPRITE_T  *old_sprites;    // for reloading sprites and translating old ids
size_t     old_spr_count;

static size_t spr_alloc;
static char   filename[100];
static int    line_num;
static FILE  *f;

#define SPR_MAX_TOKENS 100
static char *tokens[SPR_MAX_TOKENS];


static SPRITE_T *new_sprite(int texnum,const char *name,const SPRITE_T *base);
static int fail(const char *msg);
static int tokenize(char *s);
static int read_anchor(int i,SPRITE_T *arg);
static void read_num(int *num, const char *token);


void sprblit( SPRITE_T *spr, int x, int y )
{
  if( !spr ) return;

  int zlo = DEPTH_OF(y) + spr->bump;
  int zhi;

  if( spr->flags & SPRF_FLOOR )
    zhi = zlo - spr->rec.h;
  else
    zhi = zlo + spr->rec.h;

  SJGL_SetTex( spr->texnum );
  SJGL_BlitSkew( &spr->rec, x-spr->ancx, y-spr->ancy, zlo, zhi );
}


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
  int gridcols  =  1; // just to avoid compiler warning
  int gridoffs  =  0;
  int stretch   =  0;
  int stretch_t =  0;
  int stretch_r =  0;
  int stretch_b =  0;
  int stretch_l =  0;
  int piping    =  0;
  int also      =  0;
  int gridstart = -1;
  SPRITE_T defs = {0, 0, 0, NULL, {0, 0, 32, 32}, 16, 32, 0, NULL};
  SPRITE_T gdefs;
  SPRITE_T prev_spr = defs;

  line_num = 0;

  for( ;; ) { // each line in the file
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
    SPRITE_T *spr      = NULL;
    SPRITE_T *targ     = NULL;

    if( count < 1 )
      continue;

    // read the first token, which should be a command or identifier
    if( tokens[i][0] != '.' ) { // sprite name found
      if( also ) {
        spr = new_sprite(texnum,tokens[i],&prev_spr);
        also = 0;
      } else if( mode == GRIDITEM ) {
        spr = new_sprite(texnum,tokens[i],&gdefs);
        spr->rec.x += gdefs.rec.w * (gridoffs % gridcols);
        spr->rec.y += gdefs.rec.h * (gridoffs / gridcols);
        gridoffs++;
      } else {
        spr = new_sprite(texnum,tokens[i],&defs);
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

      // set special properties of the first sprite in the grid, now that the grid is complete
      if( gridstart>=0 ) {
        SPRITE_T *start = sprites + gridstart;
        start->more = calloc( 1, sizeof *(start->more) );
        start->more->gridwide = gridcols;
        start->more->gridlast = spr_count-1;
        start->more->stretch = stretch;
        start->more->piping = piping;
        if( stretch ) {
          start->more->stretch_t = stretch_t;
          start->more->stretch_r = stretch_r;
          start->more->stretch_b = stretch_b;
          start->more->stretch_l = stretch_l;
        }
      }

      gridstart = -1;
      mode = NOMORE;
      stretch = 0;
      piping = 0;

    } else if( !strcmp(tokens[i],".") ) {
      if( count > 2 )
        return fail("load_sprites: too many tokens after . command");
      i++;
      gridoffs += (count == 2 ? atoi(tokens[i]) : 1);

    } else if( !strcmp(tokens[i],".also") ) {
      if( count > 1 )
        return fail("unexpected tokens after .also command");
      also = 1;

    } else {
      SJC_Write("Unknown command: %s",tokens[i]);
      return fail("load_sprites: unknown command");
    }

    while( ++i < count ) {
      if( isdigit(tokens[i][0]) || tokens[i][0]=='-' ) {
        if( count-i != 2 && count-i != 4 && count-i < 6 )
          return fail("Expecting 2, 4 or 6+ values, when no name found");

        read_num(&targ->rec.x, tokens[  i]);
        read_num(&targ->rec.y, tokens[++i]);  if( i>=count-1 ) break;
        read_num(&targ->rec.w, tokens[++i]);
        read_num(&targ->rec.h, tokens[++i]);  if( i>=count-1 ) break;

        i = read_anchor(i+1,targ);

      } else if( !strcmp(tokens[i],"pos") ) {
        if( count-i < 3 )
          return fail("Expecting 2 args for 'pos'");

        read_num(&targ->rec.x, tokens[++i]);
        read_num(&targ->rec.y, tokens[++i]);

      } else if( !strcmp(tokens[i],"size") ) {
        if( count-i < 3 )
          return fail("Expecting 2 args for 'size'");

        read_num(&targ->rec.w, tokens[++i]);
        read_num(&targ->rec.h, tokens[++i]);

      } else if( !strcmp(tokens[i],"bump") ) {
        if( count-i < 2 )
          return fail("Expecting 1 arg for 'bump'");

        read_num(&targ->bump, tokens[++i]);

      } else if( !strcmp(tokens[i],"flange") ) {
        if( count-i < 2 )
          return fail("Expecting 1 arg for 'flange'");

        read_num(&targ->flange, tokens[++i]);
        targ->flags |= SPRF_FLOOR;

      } else if( !strcmp(tokens[i],"anchor") ) {
        if( count-i < 3 )
          return fail("Expecting 2 args for 'anchor'");

        i = read_anchor(i+1,targ);

      } else if( !strcmp(tokens[i],"cols") ) {
        if( count-i < 2 )
          return fail("Expecting column count after 'cols'");

        gridcols = atoi(tokens[++i]);

        if( gridcols < 1 )
          return fail("Column count must be a number greater than zero");

      } else if( !strcmp(tokens[i],"stretch") ) {
        if( count-i < 5 )
          return fail("Expecting top, right, bottom, left sizes after 'stretch'");

        stretch = 1;
        stretch_t = atoi(tokens[++i]);
        stretch_r = atoi(tokens[++i]);
        stretch_b = atoi(tokens[++i]);
        stretch_l = atoi(tokens[++i]);

      } else if( !strcmp(tokens[i],"pipe") ) {
        piping = 1;

      } else if( !strcmp(tokens[i],"tool") ) {
        if( count-i < 2 )
          return fail("Expecting tool name after 'tool'");

        i++;
        if(      !strncmp(tokens[i],"nu",2) ) { targ->flags |= TOOL_NUL ; } // undefined behavior if multiple tools are specified for a sprite!
        else if( !strncmp(tokens[i],"so",2) ) { targ->flags |= TOOL_SOL ; }
        else if( !strncmp(tokens[i],"pl",2) ) { targ->flags |= TOOL_PLAT; }
        else if( !strncmp(tokens[i],"op",2) ) { targ->flags |= TOOL_OPN ; }
        else if( !strncmp(tokens[i],"co",2) ) { targ->flags |= TOOL_COPY; }
        else if( !strncmp(tokens[i],"ps",2) ) { targ->flags |= TOOL_PSTE; }
        else if( !strncmp(tokens[i],"ob",2) ) { targ->flags |= TOOL_OBJ ; }
        else if( !strncmp(tokens[i],"er",2) ) { targ->flags |= TOOL_ERAS; }
        else if( !strncmp(tokens[i],"vi",2) ) { targ->flags |= TOOL_VIS ; }
        else return fail("Unknown tool name");

      } else if( !strcmp(tokens[i],"flipx") ) {
        targ->flags |= SPRF_FLIPX;

      } else if( !strcmp(tokens[i],"flipy") ) {
        targ->flags |= SPRF_FLIPY;

      } else if( !strcmp(tokens[i],"floor") ) {
        targ->flags |= SPRF_FLOOR;

      } else {
        SJC_Write("tokens %d: %s",i,tokens[i]);
        return fail("Unknown property name");

      }
    }

    // finish the sprite if we were creating one this iteration
    if( spr ) {
      memcpy( &prev_spr, spr, sizeof *spr );

      if(      spr->flags & SPRF_LFT ) spr->ancx += 0;
      else if( spr->flags & SPRF_CEN ) spr->ancx += spr->rec.w / 2;
      else if( spr->flags & SPRF_RGT ) spr->ancx += spr->rec.w;

      if(      spr->flags & SPRF_TOP ) spr->ancy += 0;
      else if( spr->flags & SPRF_MID ) spr->ancy += spr->rec.h / 2;
      else if( spr->flags & SPRF_BOT ) spr->ancy += spr->rec.h;

      if( spr->flags & SPRF_FLIPX ) { spr->rec.x += spr->rec.w; spr->rec.w = -spr->rec.w; }
      if( spr->flags & SPRF_FLIPY ) { spr->rec.y += spr->rec.h; spr->rec.h = -spr->rec.h; }

      // make the SM() macro (for accessing sprites FAST) work by filling in the spr_map
      int i;
      for( i=0; i<sprite_enum_max; i++ )
        if( 0==strcmp(spr->name,spr_names[i]) )
          spr_map[i] = spr_count-1;

      // remember that this was the first sprite in the grid
      if( gridstart==-1 && mode==GRIDITEM )
        gridstart = spr_count-1;
    }
  }

  fclose(f);

  return 0;
}

// reload all sprites
// old sprite data is available in old_sprites
// call unload_sprites(old_sprites,old_spr_count) soon after to avoid leaks
void reload_sprites()
{
  old_sprites   = sprites;
  old_spr_count = spr_count;

  sprites   = NULL;
  spr_count = 0;
  spr_alloc = 0;

  memset( spr_map, 0, sprite_enum_max * sizeof *spr_map );

  size_t i;
  for( i=0; i<tex_count; i++ )
    if( textures[i].filename )
      load_sprites(i);
}

// unload either old_sprites (after reloading all sprites) or sprites (when shutting down)
void unload_sprites(SPRITE_T *sprites,size_t spr_count)
{
  size_t i;
  for( i=0; i<spr_count; i++ ) {
    free(sprites[i].name);
    free(sprites[i].more);
  }
  free(sprites);
  sprites = NULL;
  spr_count = 0;
  // does not reset spr_alloc! (there's no old_spr_alloc)
}

int find_sprite_by_name(const char *name)
{
  // TODO: index this later!
  size_t i;
  for( i=0; i<spr_count; i++ )
    if( !strcmp(name,sprites[i].name) )
      return (int)i;
  return 0;
}

static SPRITE_T *new_sprite(int texnum,const char *name,const SPRITE_T *base)
{
  if( spr_count == spr_alloc ) {
    size_t new_alloc = spr_alloc < 8 ? 8 : spr_alloc*2;
    sprites = realloc( sprites, new_alloc * sizeof *sprites );
    memset( sprites + spr_alloc, 0, (new_alloc - spr_alloc) * sizeof *sprites );
    spr_alloc = new_alloc;
  }

  if( base )
    memcpy( sprites+spr_count, base, sizeof *sprites );

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
  int xcaret = 0;
  int ycaret = 0;

  char *p = tokens[i];

  if( *p!='^' ) targ->ancx = 0;

  if( isdigit(*p) || *p=='-'  ) { targ->flags &= ~SPRF_ALIGNXMASK; targ->ancx = atoi(p);                }
  else if( *p=='^'            ) {                                  xcaret = targ->ancx;                 }
  else if( !strncmp(p,"to",2) ) { targ->flags &= ~SPRF_ALIGNYMASK; targ->flags |= SPRF_TOP; flipme = 1; }
  else if( !strncmp(p,"le",2) ) { targ->flags &= ~SPRF_ALIGNXMASK; targ->flags |= SPRF_LFT;             }
  else if( !strncmp(p,"mi",2) ) { targ->flags &= ~SPRF_ALIGNYMASK; targ->flags |= SPRF_MID; flipme = 1; }
  else if( !strncmp(p,"ce",2) ) { targ->flags &= ~SPRF_ALIGNXMASK; targ->flags |= SPRF_CEN;             }
  else if( !strncmp(p,"bo",2) ) { targ->flags &= ~SPRF_ALIGNYMASK; targ->flags |= SPRF_BOT; flipme = 1; }
  else if( !strncmp(p,"ri",2) ) { targ->flags &= ~SPRF_ALIGNXMASK; targ->flags |= SPRF_RGT;             }

  while( *p && *p!='-' && *p!='+' )
    p++;
  if( *p ) targ->ancx = atoi(p) + xcaret;

  p = tokens[++i];

  if( *p!='^' ) targ->ancy = 0;

  if( isdigit(*p) || *p=='-'  ) { targ->flags &= ~SPRF_ALIGNYMASK; targ->ancy = atoi(p);                }
  else if( *p=='^'            ) {                                  ycaret = targ->ancy;                 }
  else if( !strncmp(p,"to",2) ) { targ->flags &= ~SPRF_ALIGNYMASK; targ->flags |= SPRF_TOP;             }
  else if( !strncmp(p,"le",2) ) { targ->flags &= ~SPRF_ALIGNXMASK; targ->flags |= SPRF_LFT; flipme = 1; }
  else if( !strncmp(p,"mi",2) ) { targ->flags &= ~SPRF_ALIGNYMASK; targ->flags |= SPRF_MID;             }
  else if( !strncmp(p,"ce",2) ) { targ->flags &= ~SPRF_ALIGNXMASK; targ->flags |= SPRF_CEN; flipme = 1; }
  else if( !strncmp(p,"bo",2) ) { targ->flags &= ~SPRF_ALIGNYMASK; targ->flags |= SPRF_BOT;             }
  else if( !strncmp(p,"ri",2) ) { targ->flags &= ~SPRF_ALIGNXMASK; targ->flags |= SPRF_RGT; flipme = 1; }

  while( *p && *p!='-' && *p!='+' )
    p++;
  if( *p ) targ->ancy = atoi(p) + ycaret;
  
  if( flipme ) SWAP( targ->ancx, targ->ancy, int );

  return i;
}


// Read a number from token into num, like atoi
// But if it starts with ^, add the number to num
// It is OK if there's no number after the ^
static void read_num(int *num, const char *token)
{
  if( *token!='^' ) {
    *num = atoi(token);
    return;
  }

  token++;

  if( *token!='+' && *token!='-' )
    return;

  *num += atoi(token);
}
