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


//cmd types
enum { CMDT_1LEFT = 1,
       CMDT_0LEFT,
       CMDT_1RIGHT,
       CMDT_0RIGHT,
       CMDT_1UP,
       CMDT_0UP,
       CMDT_1DOWN,
       CMDT_0DOWN,
       CMDT_1JUMP,
       CMDT_0JUMP,
       CMDT_1DASH,
       CMDT_0DASH,
       CMDT_1EPANT, //editing controls...
       CMDT_0EPANT,
       CMDT_1EPREV,
       CMDT_0EPREV,
       CMDT_1ENEXT,
       CMDT_0ENEXT,
       CMDT_1EPGDN,
       CMDT_0EPGDN };


extern int    myghost;
extern int    myghostleft;
extern int    myghosttop;
extern int    mycontext;
extern int    downx;
extern int    downy;

