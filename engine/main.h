
#ifndef SPARTOR_MAIN_H_
#define SPARTOR_MAIN_H_

#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"
#include "SDL_net.h"
#include "SDL_image.h"
#include "mod_constants.h"

#define VERSION "0.15.04.04"
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef GITCOMMIT
#define GITCOMMIT "* no commit"
#endif

#define TICKSAFRAME 30

//macros
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define TRY do{
#define HARDER }while(0);
#define SWAP(a,b,T) TRY T SWAP_tmp__ = (a); (a) = (b); (b) = SWAP_tmp__; HARDER
#define HAS(v,flags) (((v)&(flags)) == (flags))
#define COUNTOF(ident) ((sizeof (ident)) / (sizeof *(ident)))

#define safe_free(x) do { free(x); x = NULL; } while(0)
#define safe_copy(to,from) do { to = NULL; if(from) { to = malloc(strlen(from)+1); strcpy(to,from); } } while(0)

#define assert(expr) { if(!(expr)) SJC_Write( "%s(%d) Assert failed! %s", __FILE__, __LINE__, #expr ); }


#define TOKEN_PASTE_(a,b) a ## b
#define TOKEN_PASTE(a,b) TOKEN_PASTE_(a,b)
#define STRINGIFY_(a) #a
#define STRINGIFY(a) STRINGIFY_(a)


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
} flexer[OBJT_MAX];


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


typedef struct{
  float     x,y,z;
} V;


// frame buffer structures //
typedef struct{
  char      cmd;
  char      mousehi;
  char      mousex;
  char      mousey;
  short     flags;
  size_t    datasz;
  Uint8     data[MAXCMDDATA];
} FCMD_t;

typedef struct{
  short     type;
  short     flags;
  int       context;
  size_t    size;
  void     *data;
} OBJ_t;

typedef struct{
  int       dirty;
  Uint32    realfr;
  FCMD_t   *cmds;
  OBJ_t    *objs;
} FRAME_t;


// map structures //
typedef struct{
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
} INPUTNAME_t;


#define EXPOSE(T,N,A) T N A;
#define HIDE(X) X
#define STRUCT() typedef struct {
#define ENDSTRUCT(TYPE) } TOKEN_PASTE(TYPE,_t);
#include "engine_structs.h"
#include "game_structs.h"
#undef EXPOSE
#undef HIDE
#undef STRUCT
#undef ENDSTRUCT

//get a pointer to a member in 'flexible' struct -- whee polymorphism sort of
#define flex(o,memb) ((void *)((char *)((o)->data) + flexer[(o)->type].memb))



//externs
extern Uint32 ticksaframe;
extern int    maxframes;
extern int    maxobjs;
extern int    maxclients;

extern FRAME_t *fr;
extern Uint32 frameoffset; //offset to sync client with server
extern Uint32 metafr; //the frame corresponding to "now"
extern Uint32 surefr; //newest frame we are sure of (i.e. it has all its inputs and has been advanced)
extern Uint32 drawnfr;//most recently rendered frame
extern Uint32 hotfr;  //newest fully advanced frame (it's hot and fresh)
extern Uint32 cmdfr;  //newest frame with cmds inserted (possibly in future)

extern Uint32 ticks;
extern int    me;
extern int    console_open;

extern Uint32 total_time;
extern Uint32 idle_time;
extern Uint32 render_time;
extern Uint32 adv_move_time;
extern Uint32 adv_collide_time;
extern Uint32 adv_game_time;
extern Uint32 adv_frames;

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
void jogframebuffer(Uint32 newmetafr,Uint32 newsurefr);


#endif

