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

#include "console.h"

#define LEN CONSOLE_BUFLEN

static char   *buf[LEN];
static size_t  size[LEN];
static size_t  pos;
static char   *rememory[LEN];
static int     rememend;
static int     remempos;
static FILE   *logfile = NULL;

static void recall()
{
  if( remempos == rememend ) // current position
  {
    buf[0][0] = '\0';
  }
  else
  {
    free(buf[0]);
    size[0] = strlen(rememory[remempos])+1;
    buf[0] = malloc(size[0]);
    strcpy(buf[0], rememory[remempos]);
  }

  console_end();
}

void console_put(char c)
{
  if( buf[0]==NULL || strlen(buf[0])+1>=size[0] )
  {
    size[0] += 32;
    buf[0] = realloc( buf[0], size[0] );

    if( size[0] == 32 )
      buf[0][0] = '\0';
  }

  char *to   = buf[0] + pos + 1;
  char *from = buf[0] + pos;
  memmove(to, from, strlen(from) + 1);

  buf[0][pos] = c;
  pos++;
}

void console_replace(const char *s)
{
  if( buf[0]==NULL || strlen(s)+1>=size[0] )
  {
    size[0] = strlen(s) + 32;
    buf[0] = realloc( buf[0], size[0] );
  }

  strcpy(buf[0], s);
}

void console_write(const char *s, ...)
{
  static char buf_[256];
  free(buf[LEN-1]);
  memmove(buf+2, buf+1, sizeof(char*)*(LEN-2));
  memmove(size+2, size+1, sizeof(int)*(LEN-2));

  va_list args;
  va_start(args, s);
  vsnprintf(buf_, 255, s, args);
  va_end(args);

  size[1] = strlen(buf_)+1;
  buf[1] = malloc(size[1]);
  strcpy(buf[1], buf_);

  va_start(args, s);
  if( logfile )
  {
    vfprintf(logfile, s, args);
    fputc('\n', logfile);
  }
  else
  {
    vfprintf(stderr, s, args);
    fputc('\n', stderr);
  }
  va_end(args);
}

void console_debug(const char *s, ...)
{
  va_list args;
  va_start(args, s);
  vfprintf(stderr, s, args);
  fputc('\n', stderr);
  va_end(args);
}

void console_log(const char *newfile)
{
  if( logfile ) fclose(logfile);

  logfile = NULL;

  if( newfile && *newfile )
    logfile = fopen(newfile, "w");
}

void console_rub(int right)
{
  if( buf[0] == NULL ) return;

  if( right )
  {
    if( buf[0] && pos < strlen(buf[0]) )
      pos++;
    else
      return;
  }

  if( pos == 0 ) return;

  char *from = buf[0] + pos;
  char *to   = buf[0] + pos - 1;
  memmove(to, from, strlen(from) + 1);

  pos--;
}

void console_clear()
{
  if( buf[0]!=NULL )
      buf[0][0] = '\0';
}

void console_up()
{
  if( (rememend+1)%LEN == remempos )
    return; // already at oldest

  int pos = (remempos+LEN-1) % LEN;
  if( !rememory[pos] )
    return; // older entry is NULL

  remempos = pos;
  recall();
}

void console_down()
{
  if( remempos == rememend )
    return; // already at newest

  remempos = (remempos+1) % LEN;
  recall();
}

void console_left()
{
  if( pos > 0 )
    pos--;
}

void console_right()
{
  if( buf[0] && pos < strlen(buf[0]) )
    pos++;
}

void console_home()
{
  pos = 0;
}

void console_end()
{
  pos = 0;
  if( buf[0] )
    pos = strlen(buf[0]);
}

void console_copy()
{
  if( buf[0] )
    SDL_SetClipboardText(buf[0]);
}

void console_paste()
{
  char *p = SDL_GetClipboardText();
  char *q = p;
  while( p && *p )
    console_put(*p++);
  SDL_free(q);
}

char *console_submit()
{
  if( !buf[0] || !strlen(buf[0]) )
    return NULL;

  free(rememory[rememend]);
  rememory[rememend] = malloc(strlen(buf[0])+1);
  strcpy(rememory[rememend], buf[0]);
  remempos = rememend = (rememend+1) % LEN;

  free(buf[LEN-1]);
  memmove(buf+1, buf, sizeof(char*)*(LEN-1));
  memmove(size+1, size, sizeof(int)*(LEN-1));
  size[0] = 0;
  buf[0] = NULL;
  pos = 0;

  fprintf(logfile ? logfile : stderr, "%s%s\n", "> ", buf[1]);

  if( strlen(buf[1])+2 >= size[1] )
  {
    size[1] += 2;
    buf[1] = realloc(buf[1], size[1]);
  }
  memmove(buf[1]+2, buf[1], size[1]-2);
  buf[1][0] = '\1';
  buf[1][1] = ' ';

  return buf[1] + 2;
}

char **console_getbuf()
{
  return buf;
}

int console_getpos()
{
  return pos;
}

