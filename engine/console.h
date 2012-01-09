
#ifndef SPARTOR_SJCONSOLE_H_
#define SPARTOR_SJCONSOLE_H_

#include "SDL.h"
#include "font.h"

typedef struct
{
  char   *buf[200];
  size_t  size[200];
  char   *rememory[200];
  int     rememend;
  int     remempos;
} SJC_t;

extern SJC_t SJC;

void SJC_Init();
void SJC_Put(char c);
void SJC_Write(const char *s,...);
void SJC_Rub();
void SJC_Up();
void SJC_Down();
int SJC_Submit();

#endif

