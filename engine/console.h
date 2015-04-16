#ifndef SPARTOR_SJCONSOLE_H_
#define SPARTOR_SJCONSOLE_H_

#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"
#include "font.h"

#define echo SJC_Write

#define SJC_BUFLEN 200

typedef struct
{
  char   *buf[SJC_BUFLEN];
  size_t  size[SJC_BUFLEN];
  char   *rememory[SJC_BUFLEN];
  int     rememend;
  int     remempos;
} SJC_t;

extern SJC_t SJC;

void SJC_Init();
void SJC_Put(char c);
void SJC_Replace(const char *c);
void SJC_Write(const char *s,...);
void SJC_Log(const char *newfile);
void SJC_Rub();
void SJC_Clear();
void SJC_Up();
void SJC_Down();
void SJC_Copy();
void SJC_Paste();
int SJC_Submit();

#endif
