/**
 **  font
 **
 **  A simple library for drawing a 'system' font on SDL GL surfaces
 **  which requires no outside resources (images, fonts, etc.)
 **/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifndef SPARTOR_FONT_H_
#define SPARTOR_FONT_H_

#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"
#include "sjdl.h"

#define NATIVE_TEX_SZ 512

#define FONT_LEFT   -1
#define FONT_CENTER  0
#define FONT_RIGHT   1

#define drawchar(x, y, ch) font_char(1, x, y, ch)
#define drawtext(x, y, align, str, ...) font_text(1, x, y, align, str, __VA_ARGS__)

void font_init();
void font_char(int scale, int x, int y, char ch);
void font_text(int scale, int x, int y, int align, const char *str, ...);
int font_extents(const char *str, int n);

#endif

