/**
 **  SPARToR
 **  Network Game Engine
 **  Copyright (C) 2010-2015  Jer Wilson
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
#include "font.h"

#define ISCOLORCODE(s) ((s)[0]=='\\' && (s)[1]=='#' && isxdigit((s)[2]) && isxdigit((s)[3]) && isxdigit((s)[4]))
#define UNHEX(x) (unsigned char)(17*((x)>'9' ? ((x)&~('a'^'A'))-'A'+10 : (x)-'0'))

unsigned int tex = 0;
static int glyph_w = 8;
static int glyph_h = 12;
static int space[256] = {
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,   //non-printable
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,   //non-printable
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,   //<space> - </>
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,   //<0> - <?>
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,   //<@> - <O>
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,   //<P> - <_>
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,   //<`> - <o>
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0    //<p> - <127>
};
static char raw[128*128] = {
"                                                                                                                                "
"         O                                                                                                                      "
" OOOO     O      OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO  "
" O  O      O     OOOOO   O   O   O  OO   O   O   O   O   O   O   O O O   O   O   OOO O   O O O   O OOO   O O O   O   O   O   O  "
" O  O       O    OOOOO   O OOO   O O O   O OOO   O OOO   O OOO   O O O   OO OO   OOO O   O O O   O OOO   O   O   O O O   O O O  "
" O  O        O   OOOOO   O OOO   O O O   O   O   O  OO   O O O   O   O   OO OO   OOO O   O  OO   O OOO   O   O   O O O   O O O  "
" O  O       O    OOOOO   O OOO   O O O   O OOO   O OOO   O O O   O O O   OO OO   O O O   O O O   O OOO   O O O   O O O   O O O  "
" O  O      O     OOOOO   O   O   O  OO   O   O   O OOO   O   O   O O O   O   O   O   O   O O O   O   O   O O O   O O O   O   O  "
" OOOO     O      OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO  "
"         O                                                                                                                      "
"                                                                                                                                "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                                                                "
" OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO  "
" O   O   O   O   O   O   O   O   O   O   O O O   O O O   O O O   O O O   O O O   O   O   O  OO   O OOO   OO  O   OO OO   OOOOO  "
" O O O   O O O   O O O   O OOO   OO OO   O O O   O O O   O O O   O O O   O O O   OOO O   O OOO   O OOO   OOO O   O O O   OOOOO  "
" O   O   O O O   O  OO   O   O   OO OO   O O O   O O O   O   O   OO OO   O   O   OO OO   O OOO   OO OO   OOO O   OOOOO   OOOOO  "
" O OOO   O   O   O O O   OOO O   OO OO   O O O   O O O   O   O   O O O   OOO O   O OOO   O OOO   OOO O   OOO O   OOOOO   OOOOO  "
" O OOO   OO OO   O O O   O   O   OO OO   O   O   OO OO   O O O   O O O   O   O   O   O   O  OO   OOO O   OO  O   OOOOO   O   O  "
" OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO   OOOOO  "
"                                                                                                                                "
"                                                                                                                                "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                  O O              O                       O                                                                    "
"           O      O O     O O     OOO             OO       O        OO   OO                                                 O   "
"           O      O O     O O    O O O   O   O   O  O      O       O       O       O       O                                O   "
"           O             OOOOO   O O        O    O O              O         O    O O O     O                               O    "
"           O              O O     OOO      O      O               O         O     O O    OOOOO           OOOOO             O    "
"           O             OOOOO     O O    O      O O O            O         O    O O O     O                              O     "
"                          O O    O O O   O   O   O  O             O         O      O       O                              O     "
"           O              O O     OOO             OO O             O       O                       O               O     O      "
"                                   O                                OO   OO                       O                      O      "
"                                                                                                                                "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                                                                "
"  OOO      O      OOO     OOO    O       OOOOO    OOO    OOOOO    OOO     OOO                        O           O        OOO   "
" O   O    OO     O   O   O   O   O  O    O       O           O   O   O   O   O                      O             O      O   O  "
" O   O     O         O       O   O  O    OOOO    O           O   O   O   O   O                     O     OOOOO     O         O  "
" O O O     O        O      OO    OOOOO       O   OOOO       O     OOO     OOOO     O       O      O                 O       O   "
" O   O     O       O         O      O        O   O   O      O    O   O       O                     O     OOOOO     O       O    "
" O   O     O      O      O   O      O    O   O   O   O     O     O   O       O                      O             O             "
"  OOO     OOO    OOOOO    OOO       O     OOO     OOO      O      OOO     OOO      O       O         O           O         O    "
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
"                                                                                                                                "
" OOOO     OOO    OOOO     OOOO   OOOOO   O   O   O   O   O   O   O   O   O   O   OOOOO    OOO    O        OOO      O            "
" O   O   O   O   O   O   O         O     O   O   O   O   O   O   O   O   O   O       O    O      O          O     O O           "
" O   O   O   O   O   O   O         O     O   O   O   O   O   O    O O    O   O      O     O       O         O    O   O          "
" OOOO    O   O   OOOO     OOO      O     O   O   O   O   O O O     O      O O      O      O       O         O                   "
" O       O   O   O  O        O     O     O   O    O O    O O O    O O      O      O       O        O        O                   "
" O       O   O   O   O       O     O     O   O    O O    O O O   O   O     O     O        O        O        O                   "
" O        OOO    O   O   OOOO      O      OOOO     O      O O    O   O     O     OOOOO    O         O       O                   "
"             O                                                                            OOO       O     OOO            OOOOO  "
"                                                                                                                                "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                                                                "
"  O              O                   O             OOO           O         O       O     O       OOO                            "
"   O             O                   O            O              O                       O         O                            "
"    O     OOO    OOOO     OOOO    OOOO    OOO     O       OOOO   O OO     OO      OO     O   O     O     OOOO    O OO     OOO   "
"             O   O   O   O       O   O   O   O   OOOO    O   O   OO  O     O       O     O  O      O     O O O   OO  O   O   O  "
"          OOOO   O   O   O       O   O   OOOOO    O      O   O   O   O     O       O     OOO       O     O O O   O   O   O   O  "
"         O   O   O   O   O       O   O   O        O      O   O   O   O     O       O     O  O      O     O O O   O   O   O   O  "
"          OOOO   OOOO     OOOO    OOOO    OOOO    O       OOOO   O   O    OOO      O     O   O      OO   O O O   O   O    OOO   "
"                                                             O                     O                                            "
"                                                          OOO                    OO                                             "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                            OO     O     OO                     "
"                                                                                           O       O       O      OO  O  OOOO   "
"                                  O                                                        O       O       O     O  OO   O  O   "
" OOOO     OOOO    O OO    OOO    OOOO    O   O   O   O   O O O   O   O   O   O   OOOOO     O       O       O             O  O   "
" O   O   O   O    OO     O        O      O   O   O   O   O O O    O O    O   O      O     O        O        O            O  O   "
" O   O   O   O    O       OOO     O      O   O    O O    O O O     O      O O      O       O       O       O             O  O   "
" O   O   O   O    O          O    O      O  OO    O O    O O O    O O     O O     O        O       O       O             O  O   "
" OOOO     OOOO    O      OOOO      OOO    OO O     O      O O    O   O     O     OOOOO     O       O       O             OOOO   "
" O           O                                                             O                OO     O     OO                     "
" O           O                                                           OO                        O                            "
"                                                                                                                                "
// -------------------------------------------------------------------------------------------------------------------------------
"                                                                                                                                "
"                                                                                                                                "
};

//initializes the font library
void font_init()
{
        unsigned int pixels[NATIVE_TEX_SZ*NATIVE_TEX_SZ];
        unsigned int u;
        unsigned int v;

        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
                unsigned int black = 0x000000FF, white = 0xFFFFFFFF, clear = 0x00000000;
        #else
                unsigned int black = 0xFF000000, white = 0xFFFFFFFF, clear = 0x00000000;
        #endif

        for( u=0; u<128; u++ )
                for( v=0; v<128; v++ )
                        if( raw[u+v*128]!=' ' )
                                pixels[u+v*NATIVE_TEX_SZ] = white;
                        else if( (u<127 && raw[(u+1)+(v  )*128]!=' ')
                              || (u>0   && raw[(u-1)+(v  )*128]!=' ')
                              || (v<127 && raw[(u  )+(v+1)*128]!=' ')
                              || (v>0   && raw[(u  )+(v-1)*128]!=' ') )
                                pixels[u+v*NATIVE_TEX_SZ] = black;
                        else
                                pixels[u+v*NATIVE_TEX_SZ] = clear;

        //make into a GL texture
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glDeleteTextures(1, &tex);
        glGenTextures(1, &tex);

        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, NATIVE_TEX_SZ, NATIVE_TEX_SZ, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

//draws a single character with GL
void font_char(int scale, int x, int y, char ch)
{
        int sx = (ch%16)*glyph_w;
        int sy = (ch/16)*glyph_h;
        int w  = 8;
        int h  = 12;

        glBindTexture(GL_TEXTURE_2D, 0); //FIXME: hack 4 win
        glBindTexture(GL_TEXTURE_2D, tex);
        glBegin(GL_QUADS);
        glTexCoord2i(sx  , sy  ); glVertex2i(x        , y        );
        glTexCoord2i(sx+w, sy  ); glVertex2i(x+w*scale, y        );
        glTexCoord2i(sx+w, sy+h); glVertex2i(x+w*scale, y+h*scale);
        glTexCoord2i(sx  , sy+h); glVertex2i(x        , y+h*scale);
        glEnd();
}

//prints a message at location x,y with GL
//align: FONT_LEFT, FONT_CENTER, FONT_RIGHT
//str and ... work like printf
void font_text(int scale, int x, int y, int align, const char *str, ...)
{
        if( !str ) str = "<null>";

        char buf[800];
        va_list args;
        va_start(args, str);
        vsnprintf(buf, 800, str, args);
        va_end(args);

        str = buf;
        if( align >= 0 )
                x -= font_extents(str, 999) / ( align > 0 ? 1 : 2 );

        int sx;
        int sy;
        int w;
        int h = 12;

        glBindTexture(GL_TEXTURE_2D, 0); //FIXME: hack 4 win
        glBindTexture(GL_TEXTURE_2D, tex);
        glBegin(GL_QUADS);
        glColor4f(1, 1, 1, 1);
        while( *str )
        {
                if( ISCOLORCODE(str) )
                {
                        glColor3ub( UNHEX(str[2]), UNHEX(str[3]), UNHEX(str[4]) );
                        if( str[5] )
                        {
                                str += 5;
                                continue;
                        }
                }

                sx = (*str%16)*glyph_w;
                sy = (*str/16)*glyph_h;
                w = space[(unsigned char)*str];
                glTexCoord2i(sx  , sy  ); glVertex2i(x        , y        );
                glTexCoord2i(sx+w, sy  ); glVertex2i(x+w*scale, y        );
                glTexCoord2i(sx+w, sy+h); glVertex2i(x+w*scale, y+h*scale);
                glTexCoord2i(sx  , sy+h); glVertex2i(x        , y+h*scale);
                x += w * scale;
                str++;
        }
        glEnd();
}

//returns number of pixels text will consume horizontally
//non-printable characters will cause weird behavior
int font_extents(const char *str, int n)
{
        int ext = 0;
        if( str==NULL )
                return 0;

        while( *str && n-- > 0 )
        {
                if( ISCOLORCODE(str) && str[5] )
                {
                        str += 5;
                        n -= 4;
                        continue;
                }
                ext += space[(unsigned char)*str++];
        }
        return ext;
}
