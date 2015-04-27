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


#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
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

  if( tex >= tex_count ) { /*echo("Attempted to set invalid texture (%d/%d)",tex,tex_count);*/ return; }

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

  glBegin(GL_QUADS);

  glTexCoord2i(s->x     , s->y     ); glVertex3i(x , y , zhi);
  glTexCoord2i(s->x+s->w, s->y     ); glVertex3i(x2, y , zhi);
  glTexCoord2i(s->x+s->w, s->y+s->h); glVertex3i(x2, y2, zlo);
  glTexCoord2i(s->x     , s->y+s->h); glVertex3i(x , y2, zlo);

  glEnd();

  return 0;
}

int SJGL_Box3D(SPRITE_T *spr, int x, int y, int z)
{
  y += spr->bump;

  x += 12; // FIXME: hack for centering for now
  z += 12;

  REC *s = &spr->rec;
  int x2 = x - 24;
  int y0 = y - spr->flange;
  int y2 = y0 + s->h - 24;
  int z2 = z - 24;
  int syfl = s->y + spr->flange;

  //      c        c
  //    / | \      | flange
  //   d  a  e     a           a        p--a--q
  //   |/   \|                 |        |/   \|
  //   b     f     b-----f 23  | 24     b     f
  //   |\   /|                 |        |\   /|
  //   k  g  n                 g        r--g--s
  //    \ | /
  //      m

  #define d_ glTexCoord2i(s->x+s->w, s->y+12     ); glVertex3i(x2,y0,z );
  #define c_ glTexCoord2i(s->x+23  , s->y        ); glVertex3i(x2,y0,z2);
  #define e_ glTexCoord2i(s->x     , s->y+12     ); glVertex3i(x ,y0,z2);
  #define g_ glTexCoord2i(s->x+23  , syfl+24     ); glVertex3i(x ,y ,z );
  #define b_ glTexCoord2i(s->x+s->w, syfl+12     ); glVertex3i(x2,y ,z );
  #define a_ glTexCoord2i(s->x+23  , syfl        ); glVertex3i(x2,y ,z2);
  #define f_ glTexCoord2i(s->x     , syfl+12     ); glVertex3i(x ,y ,z2);
  #define n_ glTexCoord2i(s->x     , s->y+s->h-12); glVertex3i(x ,y2,z2);
  #define m_ glTexCoord2i(s->x+23  , s->y+s->h   ); glVertex3i(x ,y2,z );
  #define k_ glTexCoord2i(s->x+s->w, s->y+s->h-12); glVertex3i(x2,y2,z );
  #define p_ glTexCoord2i(s->x+s->w, s->y     ); glVertex3i(x2-12,y,z -12);
  #define q_ glTexCoord2i(s->x     , s->y     ); glVertex3i(x -12,y,z2-12);
  #define s_ glTexCoord2i(s->x     , s->y+s->h); glVertex3i(x +12,y,z -12);
  #define r_ glTexCoord2i(s->x+s->w, s->y+s->h); glVertex3i(x -12,y,z +12);

  // upper part
  glBegin(GL_TRIANGLE_FAN);
  if( spr->flange ) { a_ b_ d_ c_ e_ f_ b_ }
  else              { b_ f_ q_ p_ }
  glEnd();

  glBegin(GL_TRIANGLE_FAN);
  if( s->h > spr->flange+24 ) { g_ b_ f_ n_ m_ k_ b_ }
  else                        { b_ f_ s_ r_ }
  glEnd();

  #undef a_
  #undef b_
  #undef c_
  #undef d_
  #undef e_
  #undef f_
  #undef g_
  #undef k_
  #undef m_
  #undef n_
  #undef p_
  #undef q_
  #undef s_
  #undef r_

  return 0;
}

int SJGL_Wall3D(SPRITE_T *spr, int x, int y, int z)
{
  //FIXME: how to respect ancx without breaking things??!?!
  //x += spr->ancx;
  y -= spr->ancy;
  //z -= spr->ancx;

  REC *s = &spr->rec;

  int w = abs(s->w) * 70 / 99;
  int h = abs(s->h);
  x -= w/2;
  z -= w/2;

  int x2 = x + w;
  int y2 = y + h;
  int z2 = z + w;

  //   b-----f
  //   |     |
  //   k-----n

  if( spr->bump ) {
    glPolygonOffset(0.0,spr->bump);
    glEnable(GL_POLYGON_OFFSET_FILL);
  }

  #define b_ glTexCoord2i(s->x     , s->y     ); glVertex3i(x2,y ,z );
  #define f_ glTexCoord2i(s->x+s->w, s->y     ); glVertex3i(x ,y ,z2);
  #define k_ glTexCoord2i(s->x     , s->y+s->h); glVertex3i(x2,y2,z );
  #define n_ glTexCoord2i(s->x+s->w, s->y+s->h); glVertex3i(x ,y2,z2);

  glBegin(GL_TRIANGLE_FAN);   b_ f_ n_ k_    glEnd();

  #undef b_
  #undef f_
  #undef k_
  #undef n_

  if( spr->bump )
    glDisable(GL_POLYGON_OFFSET_FILL);

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

