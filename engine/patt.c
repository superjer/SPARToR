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


#include "mod.h"
#include "patt.h"


uint32_t patt()
{
  return genrand_int32();
}

void spatt(uint32_t seed)
{
  init_genrand(seed);
}

unsigned long sqrti(unsigned long x)  
{  
  register unsigned long n=x;
  register unsigned long root=0;

  if( n>=     0x40000000 ) { n-=     0x40000000; root =0x40000000; }
  if( n>=root+0x10000000 ) { n-=root+0x10000000; root+=0x20000000; }   root>>=1;
  if( n>=root+0x04000000 ) { n-=root+0x04000000; root+=0x08000000; }   root>>=1;
  if( n>=root+0x01000000 ) { n-=root+0x01000000; root+=0x02000000; }   root>>=1;
  if( n>=root+0x00400000 ) { n-=root+0x00400000; root+=0x00800000; }   root>>=1;
  if( n>=root+0x00100000 ) { n-=root+0x00100000; root+=0x00200000; }   root>>=1;
  if( n>=root+0x00040000 ) { n-=root+0x00040000; root+=0x00080000; }   root>>=1;
  if( n>=root+0x00010000 ) { n-=root+0x00010000; root+=0x00020000; }   root>>=1;
  if( n>=root+0x00004000 ) { n-=root+0x00004000; root+=0x00008000; }   root>>=1;
  if( n>=root+0x00001000 ) { n-=root+0x00001000; root+=0x00002000; }   root>>=1;
  if( n>=root+0x00000400 ) { n-=root+0x00000400; root+=0x00000800; }   root>>=1;
  if( n>=root+0x00000100 ) { n-=root+0x00000100; root+=0x00000200; }   root>>=1;
  if( n>=root+0x00000040 ) { n-=root+0x00000040; root+=0x00000080; }   root>>=1;
  if( n>=root+0x00000010 ) { n-=root+0x00000010; root+=0x00000020; }   root>>=1;
  if( n>=root+0x00000004 ) { n-=root+0x00000004; root+=0x00000008; }   root>>=1;
  if( n> root            ) {                     root+=0x00000002; }   return root>>1;
}  

