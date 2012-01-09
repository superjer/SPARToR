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

#include "mod.h"
#include "projection.h"


DECL_FUNC_T2N(TILE2NATIVE_X_DIMETRIC)  { return (x-z)*co->tileuw/2;     }
DECL_FUNC_T2N(TILE2NATIVE_Y_DIMETRIC)  { return (x+z)*co->tileuh/2 + y; }
DECL_FUNC_P2N(POINT2NATIVE_X_DIMETRIC) { return (o->x - o->z);          }
DECL_FUNC_P2N(POINT2NATIVE_Y_DIMETRIC) { return (o->x + o->z)/2 + o->y; }
DECL_FUNC_N2T(NATIVE2TILE_X_DIMETRIC)  { return (DIME_SHIFTX(co,x)+DIME_SHIFTY(co,y)*2)/co->tileuw; }
DECL_FUNC_N2T(NATIVE2TILE_Y_DIMETRIC)  { return 0;                                                  }
DECL_FUNC_N2T(NATIVE2TILE_Z_DIMETRIC)  { return (DIME_SHIFTY(co,y)-DIME_SHIFTX(co,x)/2)/co->tileuh; }

DECL_FUNC_T2N(TILE2NATIVE_X_ORTHO)  { return x*co->tileuw + co->tileuw/2; }
DECL_FUNC_T2N(TILE2NATIVE_Y_ORTHO)  { return y*co->tileuh;                }
DECL_FUNC_P2N(POINT2NATIVE_X_ORTHO) { return o->x;                        }
DECL_FUNC_P2N(POINT2NATIVE_Y_ORTHO) { return o->y;                        }
DECL_FUNC_N2T(NATIVE2TILE_X_ORTHO)  { return x/co->tileuw;                }
DECL_FUNC_N2T(NATIVE2TILE_Y_ORTHO)  { return y/co->tileuh;                }
DECL_FUNC_N2T(NATIVE2TILE_Z_ORTHO)  { return 0;                           }

FUNC_T2N TILE2NATIVE_X  = TILE2NATIVE_X_ORTHO;
FUNC_T2N TILE2NATIVE_Y  = TILE2NATIVE_Y_ORTHO;
FUNC_P2N POINT2NATIVE_X = POINT2NATIVE_X_ORTHO;
FUNC_P2N POINT2NATIVE_Y = POINT2NATIVE_Y_ORTHO;
FUNC_N2T NATIVE2TILE_X  = NATIVE2TILE_X_ORTHO;
FUNC_N2T NATIVE2TILE_Y  = NATIVE2TILE_Y_ORTHO;
FUNC_N2T NATIVE2TILE_Z  = NATIVE2TILE_Z_ORTHO;

