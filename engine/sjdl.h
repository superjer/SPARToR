/**
 **  SJDL
 **
 **  SuperJer's add-ons for SDL
 **
 **  SJDL_SetPixel() is based on code from the tutorials on libsdl.org
 **/

#ifndef SPARTOR_SJDL_H_
#define SPARTOR_SJDL_H_

#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"
#include "basictypes.h"
#include "sprite.h"


SDL_Surface *SJDL_CopyScaled(SDL_Surface *src, unsigned int flags, int scale);
void SJGL_SetTex(unsigned int tex);
int  SJGL_BlitSpr(SPRITE_T *spr, int x, int y, int z);
int  SJGL_BlitSprSkew(SPRITE_T *spr, int x, int y, int zlo, int zhi);
int  SJGL_Blit(REC *s, int x, int y, int z);
int  SJGL_BlitSkew(REC *s, int x, int y, int zlo, int zhi);
int  SJGL_Box3D( SPRITE_T *spr, int x, int y, int z);
int  SJGL_Wall3D(SPRITE_T *spr, int x, int y, int z);
void SJDL_SetPixel(SDL_Surface *surf, int x, int y, unsigned char  R, unsigned char  G, unsigned char  B);
void SJDL_GetPixel(SDL_Surface *surf, int x, int y, unsigned char *R, unsigned char *G, unsigned char *B);
GLenum SJDL_GLFormatOf(SDL_Surface *surf);

#endif
