#ifndef SPARTOR_MAIN_H_
#define SPARTOR_MAIN_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"
#include "SDL_net.h"
#include "SDL_image.h"
#include "mod_constants.h"

#define VERSION "0.16.02.20"
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef GITCOMMIT
#define GITCOMMIT "* no commit"
#endif

#ifndef GAME
#define GAME "GAME"
#endif

#define TICKSAFRAME 16667

//macros
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define SWAP(a,b,T) do{ T SWAP_tmp__ = (a); (a) = (b); (b) = SWAP_tmp__; }while(0)
#define HAS(v,flags) (((v)&(flags)) == (flags))
#define COUNTOF(ident) ((sizeof (ident)) / (sizeof *(ident)))

#define safe_free(x) do { free(x); x = NULL; } while(0)
#define safe_copy(to,from) do { to = NULL; if(from) { to = malloc(strlen(from)+1); strcpy(to,from); } } while(0)

#define assert(expr) { if(!(expr)) echo( "%s(%d) Assert failed! %s", __FILE__, __LINE__, #expr ); }

#define BEGIN "structbegin.h"
#define END "structend.h"

#define TOKEN_PASTE_(a,b) a ## b
#define TOKEN_PASTE(a,b) TOKEN_PASTE_(a,b)
#define STRINGIFY_(a) #a
#define STRINGIFY(a) STRINGIFY_(a)

// list of object types
enum {
        #define EXPOSE(T,N,A)
        #define HIDE(X)
        #define STRUCT() TOKEN_PASTE(TYPE,_type),
        #define ENDSTRUCT(TYPE)
        #include "engine_structs.h"
        #include "game_structs.h"
        #undef EXPOSE
        #undef HIDE
        #undef STRUCT
        #undef ENDSTRUCT
        MAX_TYPES
};

struct {
        const char *name;
        ptrdiff_t pos;
        ptrdiff_t vel;
        ptrdiff_t hull;
        ptrdiff_t pvel;
        ptrdiff_t model;
        ptrdiff_t refcount;
#ifdef FLEXER_EXTRAS
        FLEXER_EXTRAS
#endif
} flexer[MAX_TYPES];

//OBJect Flags
#define OBJF_POS  (1<< 0) //has position
#define OBJF_VEL  (1<< 1) //has velocity
#define OBJF_VIS  (1<< 2) //is visible
#define OBJF_HULL (1<< 3) //has a hull
#define OBJF_CLIP (1<< 4) //clips against solids
#define OBJF_PLAT (1<< 5) //acts as a platform
#define OBJF_PVEL (1<< 6) //has player-controlled velocity
#define OBJF_DEL  (1<< 7) //object is marked for deletion
#define OBJF_BNDX (1<< 8) //clips against context edge X-wise
#define OBJF_BNDZ (1<< 9) //clips against context edge Z-wise
#define OBJF_BNDT (1<<10) //clips against context top edge
#define OBJF_BNDB (1<<11) //clips against context bottom edge
#define OBJF_REFC (1<<12) //refcount

//CoMmanD Flags
#define CMDF_NEW  (1<< 0) //new client connect
#define CMDF_QUIT (1<< 1) //disconenct
#define CMDF_DATA (1<< 2) //command has associated data

//Context Block Flags
#define CBF_SOLID (1<< 0) //solid
#define CBF_PLAT  (1<< 1) //platform
#define CBF_NULL  (1<< 2) //(dmap only) delta data not present
#define CBF_VIS   (1<< 3) //is visible

//Projection mode for contexts
#define ORTHOGRAPHIC 1
#define DIMETRIC     2

typedef struct {
        float     x, y, z;
} V;

// frame buffer structures //
typedef struct {
        char      cmd;
        char      mousehi;
        char      mousex;
        char      mousey;
        short     flags;
        size_t    datasz;
        Uint8     data[MAXCMDDATA];
} FCMD_t;

typedef struct {
        short     type;
        short     flags;
        int       context;
        size_t    size;
        void     *data;
} object;

typedef struct {
        int       dirty;
        Uint32    realfr;
        FCMD_t   *cmds;
        object   *objs;
} FRAME_t;

// map structures //
typedef struct {
        short     flags;
        int       spr;
        Uint8     data[CBDATASIZE];
} CB;

// texture structures //
typedef struct {
        char   *filename;
        int     generated;
        GLuint  glname;
} TEX_T;

typedef struct {
        char   name[100];
        int    num;
} SYS_TEX_T;

// input structures //
typedef struct {
        char   name[16];
        int    presscmd;
        int    releasecmd;
} INPUTNAME;

//macro all object struct definitions
#define EXPOSE(T,N,A) T N A;
#define HIDE(X) X
#define STRUCT() typedef struct {
#define ENDSTRUCT(TYPE) } TYPE;
#include "engine_structs.h"
#include "game_structs.h"
#undef EXPOSE
#undef HIDE
#undef STRUCT
#undef ENDSTRUCT

#define ARGS_ADVANCE (int objid, unsigned int a, unsigned int b, object *oa, object *ob)
#define ARGS_DRAW (int objid, unsigned int vidfr, object *o, context *co)

#define advance_object_sig(T) void TOKEN_PASTE(advance_object_,T) ARGS_ADVANCE
#define draw_object_sig(T) void TOKEN_PASTE(draw_object_,T) ARGS_DRAW

//macro all advance_object and draw_object prototypes
#define EXPOSE(T,N,A)
#define HIDE(X)
#define STRUCT()                                      \
        void TOKEN_PASTE(advance_object_,TYPE) ARGS_ADVANCE; \
        void TOKEN_PASTE(draw_object_,TYPE) ARGS_DRAW;
#define ENDSTRUCT(TYPE)
#include "engine_structs.h"
#include "game_structs.h"
#undef EXPOSE
#undef HIDE
#undef STRUCT
#undef ENDSTRUCT

//get a pointer to a member in 'flexible' struct -- whee polymorphism sort of
#define flex(o,memb) ((void *)((char *)((o)->data) + flexer[(o)->type].memb))

//externs
extern unsigned long ticksaframe;
extern int    maxframes;
extern int    maxobjs;
extern int    maxclients;

extern FRAME_t *fr;
extern unsigned int frameoffset; //offset to sync client with server
extern unsigned int metafr;  //the frame corresponding to "now"
extern unsigned int surefr;  //newest frame we are sure of (i.e. it has all its inputs and has been advanced)
extern unsigned int drawnfr; //most recently rendered frame
extern unsigned int hotfr;   //newest fully advanced frame (it's hot and fresh)
extern unsigned int cmdfr;   //newest frame with cmds inserted (possibly in future)

extern unsigned long ticks;
extern int    me;
extern int    console_open;

extern unsigned long total_time;
extern unsigned long idle_time;
extern unsigned long render_time;
extern unsigned long adv_move_time;
extern unsigned long adv_collide_time;
extern unsigned long adv_game_time;
extern unsigned long adv_frames;
extern unsigned long pump_time;
extern unsigned long slough_time;

extern int    eng_realtime;

//prototypes
void toggleconsole();
void advance();
int  findfreeslot(int frame1);
void clearframebuffer();
void cleanup();

//frame setters
void setmetafr( Uint32 to);
void setsurefr( Uint32 to);
void setdrawnfr(Uint32 to);
void sethotfr(  Uint32 to);
void setcmdfr(  Uint32 to);
void jogframebuffer(Uint32 newmetafr, Uint32 newsurefr);

//timing
Uint64 getticks();

#endif
