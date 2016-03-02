// DO NOT USE INCLUDE GUARDS HERE

#define TYPE empty
#include BEGIN
HIDE(
        int decoy;
)
#include END

#define TYPE context
#include BEGIN
EXPOSE(int, refcount, ) // for memory management
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
#include END

#define TYPE popup
#include BEGIN
EXPOSE(V, pos, )
EXPOSE(V, hull, [2])
HIDE(
        int visible;
        int enabled;
        int active;
        unsigned int layer;
        int (*click)(object *obj);
        char *text;
)
#include END
