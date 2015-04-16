/**
 **  SuperJer Font
 **
 **  A simple library for drawing a 'system' font on SDL GL surfaces
 **  which requires no outside resources (images, fonts, etc.)
 **/

#ifndef SPARTOR_SJFONT_H_
#define SPARTOR_SJFONT_H_

#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"
#include "sjdl.h"

#define NATIVE_TEX_SZ 512

#define SJF_LEFT   -1
#define SJF_CENTER  0
#define SJF_RIGHT   1

typedef struct
{
  GLuint tex;
  int w;
  int h;
  int pitch;
  int space[256];
  char raw[128*128];
} SJF_t;

extern SJF_t SJF;

void SJF_Init();
void SJF_DrawChar(int x, int y, char ch);
void SJF_DrawText(int x, int y, int align, const char *str, ...);
int SJF_TextExtents(const char *str);

#endif

