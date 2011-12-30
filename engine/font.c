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

#define ISCOLORCODE(s) ((s)[0]=='\\' && (s)[1]=='#' && isxdigit((s)[2]) && isxdigit((s)[3]) && isxdigit((s)[4]))
#define UNHEX(x) (unsigned char)(17*((x)>'9' ? ((x)&~('a'^'A'))-'A'+10 : (x)-'0'))


SJF_t SJF = {0,8,12,128,
  { 0, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //non-printable
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //non-printable
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,   //<space> - </>
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,   //<0> - <?>
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,   //<@> - <O>
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,   //<P> - <_>
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,   //<`> - <o>
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0 }, //<p> - <127>
{
"                                                                                                                                "
"                                                                                                                                "
" OOOO            OOOOO   OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO   "
" O  O            OOOOO   O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O     OOO    OOOOO   O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O     OOO    OOOOO   O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O     OOO    OOOOO   O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O            OOOOO   O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" OOOO            OOOOO   OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO   "
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
"                  O O              O                       O        OO   OO                                                  O  "
"           O      O O     O O     OOO    OO       OO       O       O       O                                                 O  "
"           O      O O     O O    O O O   OO  O   O  O      O      O         O    O   O     O                                O   "
"           O             OOOOO   O O        O    O O              O         O     O O      O                                O   "
"           O              O O     OOO      O      O               O         O    OOOOO   OOOOO           OOOOO             O    "
"           O             OOOOO     O O    O      O O O            O         O     O O      O                              O     "
"                          O O    O O O   O  OO   O  O             O         O    O   O     O                              O     "
"           O              O O     OOO       OO    OO O             O       O                       O               O     O      "
"                                   O                                OO   OO                       O                      O      "
"                                                                                                                                "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                                                                "
"  OOO      O      OOO     OOO    O       OOOOO    OOO    OOOOO    OOO     OOO                        O           O        OOO   "
" O   O    OO     O   O   O   O   O   O   O       O           O   O   O   O   O                      O             O      O   O  "
" O   O   O O         O       O   O   O   OOOO    O           O   O   O   O   O                     O     OOOOO     O         O  "
" O O O     O        O      OO    OOOOO       O   OOOO       O     OOO     OOOO     O       O      O                 O       O   "
" O   O     O       O         O       O       O   O   O      O    O   O       O                     O     OOOOO     O       O    "
" O   O     O      O      O   O       O   O   O   O   O     O     O   O       O                      O             O             "
"  OOO    OOOOO   OOOOO    OOO        O    OOO     OOO      O      OOO     OOO      O       O         O           O         O    "
"                                                                                          O                                     "
"                                                                                                                                "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                                                                "
"  OOO     OOO    OOOO     OOO    OOOO    OOOOO   OOOOO    OOO    O   O    OOO        O   O   O   O       O   O   O   O    OOO   "
" O   O   O   O   O   O   O   O   O   O   O       O       O   O   O   O     O         O   O  O    O       OO OO   OO  O   O   O  "
" O OOO   O   O   O   O   O       O   O   O       O       O       O   O     O         O   O O     O       O O O   O O O   O   O  "
" O OOO   OOOOO   OOOO    O       O   O   OOO     OOOO    O       OOOOO     O         O   OO      O       O O O   O  OO   O   O  "
" O OOO   O   O   O   O   O       O   O   O       O       O  OO   O   O     O         O   O O     O       O   O   O   O   O   O  "
" O       O   O   O   O   O   O   O   O   O       O       O   O   O   O     O     O   O   O  O    O       O   O   O   O   O   O  "
"  OOO    O   O   OOOO     OOO    OOOO    OOOOO   O        OOOO   O   O    OOO     OOO    O   O   OOOOO   O   O   O   O    OOO   "
"                                                                                                                                "
"                                                                                                                                "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                          OOOO   O       OOOO                   "
" OOOO     OOO    OOOO     OOOO   OOOOO   O   O   O   O   O   O   O   O   O   O   OOOOO    O      O          O      O            "
" O   O   O   O   O   O   O         O     O   O   O   O   O   O   O   O   O   O       O    O       O         O     O O           "
" O   O   O   O   O   O   O         O     O   O   O   O   O   O    O O     O O       O     O       O         O    O   O          "
" OOOO    O   O   OOOO     OOO      O     O   O   O   O   O O O     O      O O      O      O        O        O                   "
" O       O   O   O O         O     O     O   O    O O    O O O    O O      O      O       O         O       O                   "
" O       O   O   O  O        O     O     O   O    O O    O O O   O   O     O     O        O         O       O                   "
" O        OOO    O   O   OOOO      O      OOO      O      O O    O   O     O     OOOOO    O          O      O                   "
"             O                                                                            OOOO       O   OOOO            OOOOO  "
"                                                                                                                                "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                                                                "
"  OO             O                   O              OO           O         O       O     O       OOO                            "
"   OO            O                   O             O             O                       O         O                            "
"    OO    OOO    OOOO     OOOO    OOOO    OOO    OOOOO    OOOO   OOOO     OO      OO     O   O     O     OOOO    OOOO     OOO   "
"             O   O   O   O       O   O   O   O     O     O   O   O   O     O       O     O  O      O     O O O   O   O   O   O  "
"          OOOO   O   O   O       O   O   OOOOO     O     O   O   O   O     O       O     OOO       O     O O O   O   O   O   O  "
"         O   O   O   O   O       O   O   O         O     O   O   O   O     O       O     O  O      O     O O O   O   O   O   O  "
"          OOOO   OOOO     OOOO    OOOO    OOO      O      OOOO   O   O    OOO      O     O   O      OO   O O O   O   O    OOO   "
"                                                             O                     O                                            "
"                                                          OOO                    OO                                             "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                            OO     O     OO                     "
"                                                                                           O       O       O      OO O   OOOO   "
"                                  O                                                        O       O       O     O OO    O  O   "
" OOOO     OOOO    O OO    OOOO   OOOO    O   O   O   O   O O O   O   O   O   O   OOOOO     O       O       O             O  O   "
" O   O   O   O    OO     O        O      O   O   O   O   O O O    O O    O   O      O     O        O        O            O  O   "
" O   O   O   O    O       OOO     O      O   O    O O    O O O     O      O O      O       O       O       O             O  O   "
" O   O   O   O    O          O    O      O   O    O O    O O O    O O     O O     O        O       O       O             O  O   "
" OOOO     OOOO    O      OOOO      OOO    OOOO     O      O O    O   O     O     OOOOO     O       O       O             OOOO   "
" O           O                                                             O                OO     O     OO                     "
" O           O                                                           OO                        O                            "
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
  int sx = (ch%16)*SJF.w;
  int sy = (ch/16)*SJF.h;
  int w  = 8;
  int h  = 12;

  glBindTexture(GL_TEXTURE_2D,0); //FIXME: hack 4 win
  glBindTexture(GL_TEXTURE_2D,SJF.tex);
  glBegin(GL_QUADS);
  glTexCoord2i(sx  ,sy  ); glVertex2i(x  ,y  );
  glTexCoord2i(sx+w,sy  ); glVertex2i(x+w,y  );
  glTexCoord2i(sx+w,sy+h); glVertex2i(x+w,y+h);
  glTexCoord2i(sx  ,sy+h); glVertex2i(x  ,y+h);
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
  glColor4f(1,1,1,1);
  while( *str )
  {
    // colors?
    if( ISCOLORCODE(str) ) {
      glColor3ub( UNHEX(str[2]), UNHEX(str[3]), UNHEX(str[4]) );
      if( str[5] ) {
        str += 5;
        continue;
      }
    }

    sx = (*str%16)*SJF.w;
    sy = (*str/16)*SJF.h;
    w = SJF.space[(Uint8)*str];
    glTexCoord2i(sx  ,sy  ); glVertex2i(x  ,y  );
    glTexCoord2i(sx+w,sy  ); glVertex2i(x+w,y  );
    glTexCoord2i(sx+w,sy+h); glVertex2i(x+w,y+h);
    glTexCoord2i(sx  ,sy+h); glVertex2i(x  ,y+h);
    x += w;
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
  while(*str) {
    if( ISCOLORCODE(str) && str[5] ) {
      str += 5;
      continue;
    }
    n += SJF.space[(Uint8)*str++];
  }
  return n;
}

