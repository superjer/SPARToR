// DO NOT USE INCLUDE GUARDS HERE

#define TYPE EMPTY
STRUCT()
  HIDE(
    int decoy;
  )
#include "endstruct.h"

#define TYPE CONTEXT
STRUCT()
  EXPOSE(int,refcount,) // for memory management
  HIDE(
    int  bsx;           // block size
    int  bsy;
    int  bsz;
    int  x;             // context dimensions
    int  y;
    int  z;
    int  tileuw;        // tile graphics "used" width/height; includes spacing or overlap
    int  tileuh;
    int  projection;    // whether to be displayed AXIS or DIMETRIC or PERSPECTIVE
    CB  *map;           // block data
    CB  *dmap;          // delta block data
  )
#include "endstruct.h"

#define TYPE POPUP
STRUCT()
  EXPOSE(V,pos,)
  EXPOSE(V,hull,[2])
  HIDE(
    int visible;
    int enabled;
    int active;
    int (*click)(OBJ_t *obj);
    char *text;
  )
#include "endstruct.h"
