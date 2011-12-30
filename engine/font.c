/**
 **  SPARToR 
 **  Network Game Engine
 **  Copyright (C) 2010-2011  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include <GL/glew.h>
#include "SDL.h"
#include "font.h"

SJF_t SJF = {0,8,12,128,
  { 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //non-printable
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //non-printable
    6, 3, 5, 7, 7, 6, 7, 3, 5, 5, 6, 7, 3, 6, 3, 6,   //<space> - </>
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 3, 5, 6, 5, 7,   //<0> - <?>
    7, 6, 6, 6, 6, 6, 6, 6, 6, 5, 6, 6, 6, 7, 7, 6,   //<@> - <O>
    6, 6, 6, 6, 7, 6, 7, 7, 7, 7, 6, 5, 6, 5, 7, 7,   //<P> - <_>
    4, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 6, 5, 7, 6, 6,   //<`> - <o>
    6, 6, 6, 6, 5, 6, 7, 7, 7, 7, 6, 5, 3, 5, 8, 0 }, //<p> - <127>
{
"                                                                                                                                "
"                                                                                                                                "
" OOOO            OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO   "
" O  O            O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O    OOO     O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O    OOO     O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O    OOO     O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O            O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" OOOO            OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO   "
"                                                                                                                                "
"                                                                                                                                "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                                                                "
" OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO   "
" O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO   "
"                                                                                                                                "
"                                                                                                                                "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                                                                "
"         O       O O               O                     O         O     O                                                  O   "
"         O       O O      O O     OOO             OO     O        O       O      O  O      O                                O   "
"         O               OOOOO   O O             O  O            O         O      OO       O                               O    "
"         O                O O     OOO    O  O     OO             O         O     OOOO    OOOOO           OOOO              O    "
"         O                O O      O O     O     O  OO           O         O      OO       O                              O     "
"                         OOOOO    OOO     O      O  O             O       O      O  O      O                              O     "
"         O                O O      O     O  O     OO O             O     O                       O               O       O      "
"                                                                                                 O                       O      "
"                                                                                                                                "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                                                                "
"  OOO      O      OOO     OOO    O       OOOOO    OOO    OOOOO    OOO     OOO                                             OOO   "
" O   O    OO     O   O   O   O   O   O   O       O           O   O   O   O   O                     O             O       O   O  "
" O   O   O O         O       O   O   O   O       O           O   O   O   O   O                    O      OOOO     O          O  "
" O O O     O      OOO      OO    OOOOO   OOOO    OOOO       O     OOO     OOOO   O       O       O                 O        O   "
" O   O     O     O           O       O       O   O   O      O    O   O       O                    O      OOOO     O        O    "
" O   O     O     O       O   O       O   O   O   O   O     O     O   O       O                     O             O              "
"  OOO    OOOOO   OOOOO    OOO        O    OOO     OOO      O      OOO     OOO    O       O                                 O    "
"                                                                                         O                                      "
"                                                                                                                                "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                                                                "
"  OOO     OO     OOO      OO     OOO     OOOO    OOOO     OO     O  O    OOO       OO    O  O    O       O   O   O   O    OO    "
" O   O   O  O    O  O    O  O    O  O    O       O       O  O    O  O     O         O    O  O    O       OO OO   OO  O   O  O   "
" O OOO   O  O    O  O    O       O  O    O       O       O       O  O     O         O    O O     O       O O O   O O O   O  O   "
" O O O   OOOO    OOO     O       O  O    OOO     OOO     O OO    OOOO     O         O    OO      O       O O O   O  OO   O  O   "
" O OOO   O  O    O  O    O       O  O    O       O       O  O    O  O     O         O    O O     O       O   O   O   O   O  O   "
" O       O  O    O  O    O  O    O  O    O       O       O  O    O  O     O      O  O    O  O    O       O   O   O   O   O  O   "
"  OOO    O  O    OOO      OO     OOO     OOOO    O        OOO    O  O    OOO      OO     O  O    OOOO    O   O   O   O    OO    "
"                                                                                                                                "
"                                                                                                                                "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                                                                "
" OOO      OO     OOO      OOO    OOOOO   O  O    O   O   O   O   O   O   O   O   OOOO    OOO     O       OOO       O            "
" O  O    O  O    O  O    O         O     O  O    O   O   O   O   O   O   O   O      O    O       O         O      O O           "
" O  O    O  O    O  O    O         O     O  O    O   O   O   O    O O    O   O     O     O        O        O     O   O          "
" OOO     O  O    OOO      OO       O     O  O    O   O   O O O     O      OOO     O      O        O        O                    "
" O       O  O    O  O       O      O     O  O     O O    O O O    O O      O     O       O         O       O                    "
" O       O  O    O  O       O      O     O  O     O O    O O O   O   O     O     O       O         O       O                    "
" O        OO     O  O    OOO       O      OO       O      O O    O   O     O     OOOO    OOO        O    OOO                    "
"            O                                                                                       O                    OOOOO  "
"                                                                                                                                "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                                                                "
" O               O                  O              OO            O        O        O     O       OO                             "
"  O              O                  O             O              O                       O        O                             "
"          OO     OOO      OOO     OOO     OO     OOO      OOO    OOO     OO       OO     O  O     O      OOOO    OOO      OO    "
"         O  O    O  O    O       O  O    O  O     O      O  O    O  O     O        O     O O      O      O O O   O  O    O  O   "
"          OOO    O  O    O       O  O    OOOO     O      O  O    O  O     O        O     OO       O      O O O   O  O    O  O   "
"         O  O    O  O    O       O  O    O        O      O  O    O  O     O        O     O O      O      O O O   O  O    O  O   "
"          OOO    OOO      OOO     OOO     OOO     O       OOO    O  O     OO       O     O  O     OO     O O O   O  O     OO    "
"                                                            O                      O                                            "
"                                                          OO                     OO                                             "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                                                                "
"                                                                                           O     O       O        OO  O  OOOO   "
"                                  O                                                       O      O        O      O  OO   O  O   "
" OOO      OOO    O OO     OOO    OOO     O  O    O   O   O O O   O   O   O   O   OOOO     O      O        O              O  O   "
" O  O    O  O    OO      O        O      O  O    O   O   O O O    O O    O   O      O    O       O         O             O  O   "
" O  O    O  O    O        OO      O      O  O     O O    O O O     O      O O     OO      O               O              O  O   "
" O  O    O  O    O          O     O      O  O     O O    O O O    O O     O O    O        O      O        O              O  O   "
" OOO      OOO    O       OOO      OO      OOO      O      OOOO   O   O     O     OOOO      O     O       O               OOOO   "
" O          O                                                              O                     O                              "
" O          O                                                             O                      O                              "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                                                                "
  }};


//initializes the SuperJer Font library
void SJF_Init()
{
  Uint32 pixels[NATIVE_TEX_SZ*NATIVE_TEX_SZ];
  Uint32 u;
  Uint32 v;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  Uint32 black = 0x000000FF, white = 0xFFFFFFFF, clear = 0x00000000;
#else
  Uint32 black = 0xFF000000, white = 0xFFFFFFFF, clear = 0x00000000;
#endif

  for(u=0; u<128; u++)
    for(v=0; v<128; v++)
      if( SJF.raw[u+v*128]!=' ' )
        pixels[u+v*NATIVE_TEX_SZ] = white;
      else if( (u<127 && SJF.raw[(u+1)+(v  )*128]!=' ')
            || (u>0   && SJF.raw[(u-1)+(v  )*128]!=' ')
            || (v<127 && SJF.raw[(u  )+(v+1)*128]!=' ')
            || (v>0   && SJF.raw[(u  )+(v-1)*128]!=' ') )
        pixels[u+v*NATIVE_TEX_SZ] = black;
      else
        pixels[u+v*NATIVE_TEX_SZ] = clear;

  //make into a GL texture
  glPixelStorei(GL_UNPACK_ALIGNMENT,4);
  glDeleteTextures(1,&SJF.tex);
  glGenTextures(1,&SJF.tex);

  glBindTexture(GL_TEXTURE_2D,SJF.tex);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, NATIVE_TEX_SZ, NATIVE_TEX_SZ, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

//draws a single character with GL
void SJF_DrawChar(int x, int y, char ch)
{
  SDL_Rect s = (SDL_Rect){ (ch%16)*SJF.w, (ch/16)*SJF.h, 8, 12 };
  SDL_Rect d = (SDL_Rect){             x,             y, 8, 12 };

  glBindTexture(GL_TEXTURE_2D,0); //FIXME: hack 4 win
  glBindTexture(GL_TEXTURE_2D,SJF.tex);
  glBegin(GL_QUADS);
  glTexCoord2i(s.x    ,s.y    ); glVertex2i(d.x    ,d.y    );
  glTexCoord2i(s.x+s.w,s.y    ); glVertex2i(d.x+d.w,d.y    );
  glTexCoord2i(s.x+s.w,s.y+s.h); glVertex2i(d.x+d.w,d.y+d.h);
  glTexCoord2i(s.x    ,s.y+s.h); glVertex2i(d.x    ,d.y+d.h);
  glEnd();
}

//prints a message at location x,y with GL
//align: SJF_LEFT, SJF_CENTER, SJF_RIGHT
//str and ... work like printf
void SJF_DrawText(int x, int y, int align, const char *str, ...)
{
  if( !str ) str = "<null>";

  char buf[800];
  va_list args;
  va_start(args, str);
  vsnprintf(buf, 800, str, args);
  va_end(args);

  str = buf;
  if( align >= 0 )
    x -= SJF_TextExtents(str) / ( align > 0 ? 1 : 2 );

  int sx;
  int sy;
  int w;
  int h = 12;

  glBindTexture(GL_TEXTURE_2D,0); //FIXME: hack 4 win
  glBindTexture(GL_TEXTURE_2D,SJF.tex);
  glBegin(GL_QUADS);
  while( *str )
  {
    sx = (*str%16)*SJF.w;
    sy = (*str/16)*SJF.h;
    w = SJF.space[(Uint8)*str];
    glTexCoord2i(sx  ,sy  ); glVertex2i(x  ,y  );
    glTexCoord2i(sx+w,sy  ); glVertex2i(x+w,y  );
    glTexCoord2i(sx+w,sy+h); glVertex2i(x+w,y+h);
    glTexCoord2i(sx  ,sy+h); glVertex2i(x  ,y+h);
    x += w-1;
    str++;
  }
  glEnd();
}

//returns number of pixels text will consume horizontally
//non-printable characters will cause weird behavior
int SJF_TextExtents(const char *str)
{
  int n = 0;
  if( str==NULL )
    return 0;
  while(*str)
    n += SJF.space[(Uint8)*str++]-1;
  return n;
}

