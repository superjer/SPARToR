/**
 **  SJDL
 **
 **  SuperJer's add-ons for SDL
 **
 **  SJDL_SetPixel() is based on code from the tutorials on libsdl.org
 **/

#ifndef SPARTOR_SJDL_H_
#define SPARTOR_SJDL_H_

#include <GL/glew.h>
#include "SDL.h"


typedef struct{
  int       x,y,w,h;
} REC;


SDL_Surface *SJDL_CopyScaled(SDL_Surface *src, Uint32 flags, int scale);
void SJGL_SetTex(GLuint tex);
int  SJGL_Blit(REC *s, int x, int y, int z);
void SJDL_SetPixel(SDL_Surface *surf, int x, int y, Uint8  R, Uint8  G, Uint8  B);
void SJDL_GetPixel(SDL_Surface *surf, int x, int y, Uint8 *R, Uint8 *G, Uint8 *B);
GLenum SJDL_GLFormatOf(SDL_Surface *surf);

#endif 
