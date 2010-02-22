/**
 **  SuperJer Font
 **
 **  A simple library for drawing a 'system' font on sdl surfaces
 **  which requires no outside resources (images, fonts, etc.)
 **/

#ifndef SPARTOR_SJFONT_H_
#define SPARTOR_SJFONT_H_

#include "SDL.h"
#include "sjdl.h"

typedef struct
{
  SDL_Surface *surf;
  int w;
  int h;
  int pitch;
  int space[256];
  char raw[128*128];
} SJF_t;

extern SJF_t SJF;

void SJF_Init();
inline void SJF_DrawChar(SDL_Surface *surf, int x, int y, char c);
inline void SJF_DrawText(SDL_Surface *surf, int x, int y, const char *s);
inline int SJF_TextExtents(const char *s);

#endif

