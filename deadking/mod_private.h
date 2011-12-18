//
// PRIVATE STUFF the ENGINE shouldn't KNOW ABOUT!
//


#include "SDL.h"
#include "main.h"


#define MKOBJ( ptr, t, con, f ) t ## _t *ptr;                                       \
                                slot0 = findfreeslot(b);                            \
                                fr[b].objs[slot0].type = OBJT_ ## t;                \
                                fr[b].objs[slot0].flags = f;                        \
                                fr[b].objs[slot0].context = con;                    \
                                fr[b].objs[slot0].size = sizeof *ptr;               \
                                ptr = fr[b].objs[slot0].data = malloc(sizeof *ptr)  ;
#define GETOBJ( ptr, t, n )     t ## _t *sw = fr[b].objs[(n)].data;           \
                                assert( fr[b].objs[(n)].type == OBJT_ ## t )  ;
#define FOBJ( n )               fr[b].objs[(n)]

//tool textures
enum { TOOL_NUL = 1,
       TOOL_SOL,
       TOOL_PLAT,
       TOOL_OPN,
       TOOL_COPY,
       TOOL_PSTE,
       TOOL_OBJ,
       TOOL_ERAS,
       TOOL_VIS,
       TOOL_MASK = 255 };


//cmd types
enum { CMDT_NONE = 0,

       CMDT_1LEFT,
       CMDT_0LEFT,
       CMDT_1RIGHT,
       CMDT_0RIGHT,
       CMDT_1UP,
       CMDT_0UP,
       CMDT_1DOWN,
       CMDT_0DOWN,

       CMDT_1NW,
       CMDT_0NW,
       CMDT_1NE,
       CMDT_0NE,
       CMDT_1SW,
       CMDT_0SW,
       CMDT_1SE,
       CMDT_0SE,

       CMDT_1SEL,
       CMDT_0SEL,
       CMDT_1BACK,
       CMDT_0BACK,

       CMDT_0CON,
       CMDT_1CON,

       CMDT_1EPANT, //editing controls...
       CMDT_0EPANT,
       CMDT_1EPREV,
       CMDT_0EPREV,
       CMDT_1ENEXT,
       CMDT_0ENEXT,
       CMDT_1EPGUP,
       CMDT_0EPGUP,
       CMDT_1EPGDN,
       CMDT_0EPGDN,
       CMDT_1ELAY0,
       CMDT_0ELAY0,
       CMDT_1ELAY1,
       CMDT_0ELAY1,
       CMDT_1ELAY2,
       CMDT_0ELAY2 };


extern int    myghost;
extern int    mycontext;
extern int    downx;
extern int    downy;

