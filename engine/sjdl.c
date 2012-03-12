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
#include "mod.h"
#include "sjdl.h"

//create a new surface by copying and scaling another
SDL_Surface *SJDL_CopyScaled(SDL_Surface *src,Uint32 flags,int scale)
{
  int u,v,i,j;
  Uint8 r,g,b;
  SDL_Surface *dst = SDL_CreateRGBSurface(flags,src->w*scale,src->h*scale,
                                          src->format->BitsPerPixel,
                                          src->format->Rmask,
                                          src->format->Gmask,
                                          src->format->Bmask,
                                          src->format->Amask);
  SDL_LockSurface(src);
  SDL_LockSurface(dst);
  for(u=0; u<src->w; u++)
    for(v=0; v<src->h; v++) {
      SJDL_GetPixel(src, u, v, &r, &g, &b);
      for(i=0; i<scale; i++)
        for(j=0; j<scale; j++)
          SJDL_SetPixel(dst, u*scale+i, v*scale+j,  r,  g,  b);
    }
  SDL_UnlockSurface(dst);
  SDL_UnlockSurface(src);
  return dst;
}


//sets the current texture unless it is already set
void SJGL_SetTex(GLuint tex)
{
  static GLuint prev = (GLuint)-1;
  if( prev==tex ) return;
  prev = tex;
  if( prev==(GLuint)-1 ) return;

  glBindTexture(GL_TEXTURE_2D,0); //FIXME: hack 4 win, suddenly also useful for handling an invalid tex

  if( tex >= tex_count ) { SJC_Write("Attempted to set invalid texture (%d/%d)",tex,tex_count); return; }

  glBindTexture( GL_TEXTURE_2D, textures[tex].glname );
}


// wrapper for SJFL_Blit when there's only one z
int SJGL_Blit(REC *s, int x, int y, int z)
{
  return SJGL_BlitSkew(s,x,y,z,z);
}


//uses GL to do draw a sprite
int SJGL_BlitSkew(REC *s, int x, int y, int zlo, int zhi)
{
  int x2 = ( s->w > 0 ? x+s->w : x-s->w );
  int y2 = ( s->h > 0 ? y+s->h : y-s->h );

  int rhi=0, ghi=0, bhi=0;
  int rlo=0, glo=0, blo=0;

  if( m_showdepth ) {
    int grad;
    int sect;

    #define GET_COLOR(z,r,g,b)                                   \
      grad = z + 512;                                            \
      sect = (grad / 256 + 70) % 7;                              \
      grad = grad % 256;                                         \
                                                                \
      switch( sect ) {                                           \
        case 0: r = grad;                                 break; \
        case 1: r = 255;        b = grad;                 break; \
        case 2: r = 255 - grad; b = 255;                  break; \
        case 3:                 b = 255;        g = grad; break; \
        case 4:                 b = 255 - grad; g = 255;  break; \
        case 5: r = grad;                       g = 255;  break; \
        case 6: r = 255;        b = grad;       g = 255;  break; \
      }

    GET_COLOR(zlo,rlo,glo,blo);
    GET_COLOR(zhi,rhi,ghi,bhi);

    glBindTexture(GL_TEXTURE_2D,0);
  }

  glBegin(GL_QUADS);
  if( m_showdepth ) glColor3ub(rhi,ghi,bhi);
  glTexCoord2i(s->x     , s->y     ); glVertex3i(x , y , zhi);
  glTexCoord2i(s->x+s->w, s->y     ); glVertex3i(x2, y , zhi);
  if( m_showdepth ) glColor3ub(rlo,glo,blo);
  glTexCoord2i(s->x+s->w, s->y+s->h); glVertex3i(x2, y2, zlo);
  glTexCoord2i(s->x     , s->y+s->h); glVertex3i(x , y2, zlo);
  glEnd();

  return 0;
}


//sets a pixel on an sdl surface
void SJDL_SetPixel(SDL_Surface *surf, int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  Uint32 color = SDL_MapRGB(surf->format, R, G, B);
  switch(surf->format->BytesPerPixel) {
  case 1: // 8-bpp
    {
      Uint8 *bufp;
      bufp = (Uint8 *)surf->pixels + y*surf->pitch + x;
      *bufp = color;
    }
    break;
  case 2: // 15-bpp or 16-bpp
    {
      Uint16 *bufp;
      bufp = (Uint16 *)surf->pixels + y*surf->pitch/2 + x;
      *bufp = color;
    }
    break;
  case 3: // 24-bpp mode, usually not used
    {
      Uint8 *bufp;
      bufp = (Uint8 *)surf->pixels + y*surf->pitch + x * 3;
      if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
      {
        bufp[0] = color;
        bufp[1] = color >> 8;
        bufp[2] = color >> 16;
      } else {
        bufp[2] = color;
        bufp[1] = color >> 8;
        bufp[0] = color >> 16;
      }
    }
    break;
  case 4: // 32-bpp
    {
      Uint32 *bufp;
      bufp = (Uint32 *)surf->pixels + y*surf->pitch/4 + x;
      *bufp = color;
    }
    break;
  }
} 


//gets a pixel on an sdl surface
void SJDL_GetPixel(SDL_Surface *surf, int x, int y, Uint8 *R, Uint8 *G, Uint8 *B)
{
  Uint32 color;
  switch(surf->format->BytesPerPixel) {
  case 1: // 8-bpp
    {
      Uint8 *bufp;
      bufp = (Uint8 *)surf->pixels + y*surf->pitch + x;
      color = *bufp;
    }
    break;
  case 2: // 15-bpp or 16-bpp
    {
      Uint16 *bufp;
      bufp = (Uint16 *)surf->pixels + y*surf->pitch/2 + x;
      color = *bufp;
    }
    break;
  case 3: // 24-bpp mode, usually not used
    {
      Uint8 *bufp;
      bufp = (Uint8 *)surf->pixels + y*surf->pitch + x * 3;
      if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
        color  = bufp[0] | (bufp[1] << 8) | (bufp[2] << 16);
      else
        color  = bufp[2] | (bufp[1] << 8) | (bufp[0] << 16);
    }
    break;
  case 4: // 32-bpp
    {
      Uint32 *bufp;
      bufp = (Uint32 *)surf->pixels + y*surf->pitch/4 + x;
      color = *bufp;
    }
    break;
  }
  SDL_GetRGB(color, surf->format, R, G, B);
} 


// returns the equivalent opengl format of an sdl surface
GLenum SJDL_GLFormatOf(SDL_Surface *surf)
{
  if( surf->format->Amask > 0 ) {
    if( surf->format->Rmask > surf->format->Bmask )
      return (SDL_BYTEORDER==SDL_LIL_ENDIAN ? GL_BGRA : GL_RGBA);
    else
      return (SDL_BYTEORDER==SDL_LIL_ENDIAN ? GL_RGBA : GL_BGRA);
  } else {
    if( surf->format->Rmask > surf->format->Bmask )
      return (SDL_BYTEORDER==SDL_LIL_ENDIAN ? GL_BGR : GL_RGB);
    else
      return (SDL_BYTEORDER==SDL_LIL_ENDIAN ? GL_RGB : GL_BGR);
  }
}

