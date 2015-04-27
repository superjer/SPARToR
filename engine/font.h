/**
 **  SuperJer Font
 **
 **  A simple library for drawing a 'system' font on SDL GL surfaces
 **  which requires no outside resources (images, fonts, etc.)
 **/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

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

#define SJF_DrawChar(x, y, ch) SJF_DrawCharScaled(1, x, y, ch)
#define SJF_DrawText(x, y, align, str, ...) SJF_DrawTextScaled(1, x, y, align, str, __VA_ARGS__)

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
void SJF_DrawCharScaled(int scale, int x, int y, char ch);
void SJF_DrawTextScaled(int scale, int x, int y, int align, const char *str, ...);
int SJF_TextExtents(const char *str, int n);

#endif

