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

#include "console.h"


SJC_t SJC = {{0},{0},{0},0,0};


static void SJC_Log(const char *prefix,const char *s);


void SJC_Put(char c)
{
  int n;
  if( SJC.buf[0]==NULL || strlen(SJC.buf[0])>=SJC.size[0]-1 ) {
    SJC.buf[0] = realloc( SJC.buf[0], SJC.size[0]+32 );
    if( SJC.size[0]==0 )
      SJC.buf[0][0] = '\0';
    SJC.size[0] += 32;
  }
  n = strlen(SJC.buf[0]);
  SJC.buf[0][n] = c;
  SJC.buf[0][n+1] = '\0';
}


void SJC_Write(const char *s,...)
{
  static char buf[256];
  free(SJC.buf[199]);
  memmove(SJC.buf+2,SJC.buf+1,sizeof(char*)*198);
  memmove(SJC.size+2,SJC.size+1,sizeof(int)*198);

  va_list args;
  va_start(args,s);
  vsnprintf(buf,255,s,args);
  va_end(args);

  SJC.size[1] = strlen(buf)+3;
  SJC.buf[1] = malloc(SJC.size[1]);
  SJC.buf[1][0] = (char)1;
  SJC.buf[1][1] = (char)32;
  strcpy(SJC.buf[1]+2,buf);

  SJC_Log("",buf);
}


void SJC_Rub()
{
  int n;
  if( SJC.buf[0]!=NULL ) {
    n = strlen(SJC.buf[0]);
    if( n )
      SJC.buf[0][n-1] = '\0';
  }
}


void recall()
{
  if( SJC.remempos == SJC.rememend ) { // current position
    SJC.buf[0][0] = '\0';
    return;
  }

  free(SJC.buf[0]);
  SJC.size[0] = strlen(SJC.rememory[SJC.remempos])+1;
  SJC.buf[0] = malloc(SJC.size[0]);
  strcpy(SJC.buf[0],SJC.rememory[SJC.remempos]);
}


void SJC_Up()
{
  if( (SJC.rememend+1)%200 == SJC.remempos )
    return; // already at oldest

  int pos = (SJC.remempos+199) % 200;
  if( !SJC.rememory[pos] )
    return; // older entry is NULL

  SJC.remempos = pos;
  recall();
}


void SJC_Down()
{
  if( SJC.remempos == SJC.rememend )
    return; // already at newest

  SJC.remempos = (SJC.remempos+1) % 200;
  recall();
}


int SJC_Submit()
{
  if(!SJC.buf[0] || !strlen(SJC.buf[0]))
    return 0;

  free(SJC.rememory[SJC.rememend]);
  SJC.rememory[SJC.rememend] = malloc(strlen(SJC.buf[0])+1);
  strcpy(SJC.rememory[SJC.rememend],SJC.buf[0]);
  SJC.remempos = SJC.rememend = (SJC.rememend+1) % 200;

  free(SJC.buf[199]);
  memmove(SJC.buf+1,SJC.buf,sizeof(char*)*199);
  memmove(SJC.size+1,SJC.size,sizeof(int)*199);
  SJC.size[0] = 0;
  SJC.buf[0] = NULL;

  SJC_Log("> ",SJC.buf[1]);

  return 1;
}


static void SJC_Log(const char *prefix,const char *s)
{
  static FILE *f = NULL;

  if( !f ) f = fopen("console.log","w");
  if( f ) fprintf(f,"%s%s\n",prefix,s);
}

