//
// PRIVATE STUFF the ENGINE shouldn't KNOW ABOUT!
//


#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"
#include "main.h"

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
       CMDT_0FIRE,
       CMDT_1FIRE,
       CMDT_0JUMP,
       CMDT_1JUMP,

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
       CMDT_1ELAUP,
       CMDT_0ELAUP,
       CMDT_1ELADN,
       CMDT_0ELADN,
       CMDT_1ESHOW,
       CMDT_0ESHOW,
       CMDT_1EUNDO,
       CMDT_0EUNDO };


extern int    myghost;
extern int    mycontext;

