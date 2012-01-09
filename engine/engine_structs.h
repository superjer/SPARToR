// DO NOT USE INCLUDE GUARDS HERE


#define TYPE EMPTY
STRUCT()
  HIDE(
    int decoy;
  )
#include "endstruct.h"


#define TYPE CONTEXT
STRUCT()
  HIDE(
    int  bsx;           // block size
    int  bsy;
    int  bsz;
    int  x;             // context dimensions
    int  y;
    int  z;
    int  tileuw;        // tile graphics "used" width/height; includes spacing or overlap
    int  tileuh;
    int  projection;    // whether to be displayed ORTHOGRAPHIC or DIMETRIC
    CB  *map;           // block data
    CB  *dmap;          // delta block data
  )
#include "endstruct.h"


