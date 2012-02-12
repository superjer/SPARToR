/**
 **  SPARToR
 **  Network Game Engine
 **  Copyright (C) 2010-2012  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include <GL/glew.h>
#include "SDL.h"
#include "SDL_net.h"
#include "SDL_image.h"
#include "mod.h"
#include "main.h"
#include "font.h"
#include "console.h"
#include "net.h"
#include "host.h"
#include "client.h"
#include "input.h"
#include "video.h"
#include "sprite.h"
#include "sprite_helpers.h"
#include <math.h>

#if (SDL_IMAGE_MAJOR_VERSION*1000000 + SDL_IMAGE_MINOR_VERSION*1000 + SDL_IMAGE_PATCHLEVEL)<1002008 //support SDL_image pre 1.2.8
#define IMG_INIT_PNG 1
int IMG_Init(int flags) {return flags;}
void IMG_Quit() {}
#endif

//globals
Uint32 ticksaframe = TICKSAFRAME;
int maxframes = 360;
int maxobjs = 100;
int maxclients = 32;

FRAME_t *fr;
Uint32 frameoffset;
Uint32 metafr;
Uint32 surefr;
Uint32 drawnfr;
Uint32 hotfr;
Uint32 cmdfr = 1; //DO NOT clear frame 1, it is prefilled with client-connect for local person

SDL_Surface *screen;
Uint32 ticks,newticks;
int me;
int console_open;

Uint32 total_time = 0;
Uint32 idle_time = 0;
Uint32 render_time = 0;
Uint32 adv_move_time = 0;
Uint32 adv_collide_time = 0;
Uint32 adv_game_time = 0;
Uint32 adv_frames = 0;

//runtime engine options
int eng_realtime = 0;

static const Uint32 sdlflags = SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_JOYSTICK;


static void init_flexers()
{
  memset(flexer,0,sizeof flexer);
  #define EXPOSE(T,N,A) flexer[TOKEN_PASTE(OBJT_,TYPE)].N=(ptrdiff_t)&((TOKEN_PASTE(TYPE,_t) *)0)->N;
  #define HIDE(X)
  #define STRUCT() flexer[TOKEN_PASTE(OBJT_,TYPE)].name = STRINGIFY(TYPE);
  #define ENDSTRUCT(TYPE)
  #include "engine_structs.h"
  #include "game_structs.h"
}


int main(int argc,char **argv)
{
  SDL_Event event;
  int i;
  Uint32 idle_start = 0;

  init_flexers();

  fr = calloc(sizeof(FRAME_t),maxframes);
  for(i=0;i<maxframes;i++) {
    fr[i].cmds = calloc(sizeof(FCMD_t),maxclients);
    fr[i].objs = calloc(sizeof(OBJ_t),maxobjs);
  }

  if( SDL_Init(sdlflags)<0 || !SDL_GetVideoInfo() ) { fprintf(stderr,"SDL_Init: %s\n",SDL_GetError());     exit(-1); }
  if( SDLNet_Init()<0 )                             { fprintf(stderr,"SDLNet_Init: %s\n",SDL_GetError());  exit(-2); }
  if( IMG_Init(IMG_INIT_PNG)!=IMG_INIT_PNG )        { fprintf(stderr,"IMG_Init: %s\n",SDL_GetError());     exit(-3); }

  SDL_WM_SetCaption("SPARToR " VERSION,NULL);
  SDL_Surface *iconsurf = IMG_Load("game/images/icon.png");
  SDL_WM_SetIcon(iconsurf,NULL);
  SDL_FreeSurface(iconsurf);

  SJC_Write("SPARToR v%s  Copyright (C) 2010-2012 Jer Wilson",VERSION);
  SJC_Write("Please visit github.com/superjer for updates and source code.");
  SJC_Write("");
  SJC_Write(" --->  \\#F80Type 'help' for help.\\#FFF  <---");
  SJC_Write("");

  toggleconsole();
  videoinit();
  inputinit();

  mod_setup(0);

  //main loop
  for(;;) {
    newticks = SDL_GetTicks();
    if( !eng_realtime && newticks-ticks<5 ) // give system some time to breathe if we're not too busy
      SDL_Delay(1);
    ticks = newticks;
    metafr = ticks/ticksaframe + frameoffset;
    while( SDL_PollEvent(&event) ) switch(event.type) {
      case SDL_VIDEOEXPOSE:                                                         break;
      case SDL_VIDEORESIZE:     setvideosoon(event.resize.w,event.resize.h,0,10);   break;
      case SDL_ACTIVEEVENT:     setactive(event.active.gain,event.active.state);    break;
      case SDL_KEYDOWN:         kbinput(    1, event.key.keysym );                  break;
      case SDL_KEYUP:           kbinput(    0, event.key.keysym );                  break;
      case SDL_JOYBUTTONDOWN:   joyinput(   1, event.jbutton );                     break;
      case SDL_JOYBUTTONUP:     joyinput(   0, event.jbutton );                     break;
      case SDL_JOYAXISMOTION:   axisinput(     event.jaxis );                       break;
      case SDL_MOUSEBUTTONDOWN: mouseinput( 1, event.button );                      break;
      case SDL_MOUSEBUTTONUP:   mouseinput( 0, event.button );                      break;
      case SDL_MOUSEMOTION:     mousemove(     event.motion );                      break;
      case SDL_QUIT:            cleanup();                                          break;
    }
    idle_time += SDL_GetTicks() - idle_start;
    readinput();
    if(hostsock)   host();
    if(clientsock) client();
    advance();
    render();
    idle_start = SDL_GetTicks();
  }
}


void toggleconsole()
{
  if( console_open ) {
    console_open = 0;
    SDL_EnableUNICODE(0);
    SDL_EnableKeyRepeat(0,0);
  } else {
    console_open = 1;
    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
  }
}


void advance()
{
  int i,j,r;
  char recheck[2][maxobjs]; // for collision rechecking
  findfreeslot(-1); // reset slot finder

  while(hotfr < metafr) {
    sethotfr(hotfr+1);
    Uint32 a = (hotfr-1)%maxframes; //a: frame to advance from, b: frame to advance to
    Uint32 b = (hotfr  )%maxframes;

    if( cmdfr<hotfr ) //need to clear out the cmds in forward frame since it hasn't been done yet!
      setcmdfr(hotfr);

    Uint32 adv_move_start = SDL_GetTicks();

    for(i=0;i<maxobjs;i++) { //first pass -- copy forward, move, clip with world
      OBJ_t *oa = fr[a].objs+i;
      OBJ_t *ob = fr[b].objs+i;
      free(ob->data);
      memset(ob,0,sizeof *ob);

      if(oa->type && !(oa->flags&OBJF_DEL)) {
        memcpy(ob,oa,sizeof *ob);
        ob->data = malloc(oa->size);
        memcpy(ob->data,oa->data,oa->size);
      }

      if( HAS(ob->flags,OBJF_POS|OBJF_VEL) && ob->context ) {
        CONTEXT_t *co = fr[b].objs[ob->context].data;
        V *pos  = flex(ob,pos);
        V *vel  = flex(ob,vel);
        V *pvel = (ob->flags & OBJF_PVEL) ? flex(ob,pvel) : &(V[2]){{0,0,0},{0,0,0}};
        V *hull = (ob->flags & OBJF_HULL) ? flex(ob,hull) : &(V[2]){{0,0,0},{0,0,0}};

        pos->x += vel->x + pvel->x;  //apply velocity
        pos->y += vel->y + pvel->y;
        pos->z += vel->z + pvel->z;

        //context edges left & right
        if( ob->flags & OBJF_BNDX )
        {
          if( pos->x + hull[1].x > co->x*co->bsx ) { pos->x = co->x*co->bsx - hull[1].x; vel->x = 0; }
          if( pos->x + hull[0].x <             0 ) { pos->x =             0 - hull[0].x; vel->x = 0; }
        }

        //context edges back and front
        if( ob->flags & OBJF_BNDZ )
        {
          if( pos->z + hull[1].z > co->z*co->bsz ) { pos->z = co->z*co->bsz - hull[1].z; vel->z = 0; }
          if( pos->z + hull[0].z <             0 ) { pos->z =             0 - hull[0].z; vel->z = 0; }
        }

        //context edge bottom
        if( ob->flags & OBJF_BNDB )
        {
          if( pos->y + hull[1].y > co->y*co->bsy ) { pos->y = co->y*co->bsy - hull[1].y; vel->y = 0; }
        }

        //context edge top
        if( ob->flags & OBJF_BNDT )
        {
          if( pos->y + hull[0].y <             0 ) { pos->y =             0 - hull[0].y; vel->y = 0; }
        }
      }
    }

    Uint32 adv_collide_start = SDL_GetTicks();
    adv_move_time += adv_collide_start - adv_move_start;

    for(r=0;r<40;r++) { //"recurse" up to so many times to sort out collisions
      memset(recheck[r%2],0,sizeof(recheck[0]));

      for(i=0;i<maxobjs;i++) {
        if(r!=0 && !recheck[(r+1)%2][i])
          continue;
        if( !HAS( fr[b].objs[i].flags, OBJF_POS|OBJF_VEL|OBJF_HULL ) )
          continue;

        OBJ_t *oldme = fr[a].objs+i, *newme = fr[b].objs+i;
        V *oldmepos  = flex(oldme,pos );
        V *newmepos  = flex(newme,pos );
        V *oldmevel  = flex(oldme,vel );
        V *newmevel  = flex(newme,vel );
        V *oldmehull = flex(oldme,hull);
        V *newmehull = flex(newme,hull);

        if( newme->context && (newme->flags & OBJF_CLIP) ) { //check CBs (context blocks (map tiles))
          CONTEXT_t *co = fr[b].objs[newme->context].data;
          int dnx = ((int)(newmepos->x + newmehull[0].x) / co->bsx);
          int dny = ((int)(newmepos->y + newmehull[0].y) / co->bsy);
          int dnz = ((int)(newmepos->z + newmehull[0].z) / co->bsz);
          int upx = ((int)(newmepos->x + newmehull[1].x) / co->bsx);
          int upy = ((int)(newmepos->y + newmehull[1].y) / co->bsy);
          int upz = ((int)(newmepos->z + newmehull[1].z) / co->bsz);
          int ix,iy,iz;

          for( iy=0; iy<=upy-dny; iy++ ) for( ix=0; ix<=upx-dnx; ix++ ) for( iz=0; iz<=upz-dnz; iz++ ) {
            int x = oldmevel->x>0 ? dnx+ix : upx-ix; //iteriate in an order matching direction of movement
            int y = oldmevel->y>0 ? dny+iy : upy-iy;
            int z = oldmevel->z>0 ? dnz+iz : upz-iz;

            if( x<0 || x>=co->x || y<0 || y>=co->y || z<0 || z>=co->z ) continue; //out of bounds?

            #define MAPPOS(X,Y,Z) ((Z)*co->y*co->x + (Y)*co->x + (X))
            int pos = MAPPOS(x,y,z);
            short flags = co->dmap[pos].flags;

            if( !(flags & (CBF_SOLID|CBF_PLAT)) ) continue;

            V *cbpos  = &(V){x*co->bsx,y*co->bsy,z*co->bsz};
            V *cbhull = (V[2]){{0,0,0},{co->bsx,co->bsy,co->bsz}};

            #define IS_SOLID(X,Y,Z)                                                                        \
            (                                                                                              \
              (X)>=0 && (Y)>=0 && (Z)>=0 && (X)<co->x && (Y)<co->y && (Z)<co->z &&                         \
              HAS( co->dmap[MAPPOS(X,Y,Z)].flags, CBF_SOLID )                                              \
            )

            #define ELSE_IF_HIT_THEN_MOVE_STOP(outX,outY,outZ,hullL,hullR,axis,LTGT)                       \
              else if( !IS_SOLID(outX,outY,outZ) &&                                                        \
                       cbpos->axis+cbhull[hullL].axis LTGT oldmepos->axis+oldmehull[hullR].axis )          \
              {                                                                                            \
                  newmepos->axis = cbpos->axis+cbhull[hullL].axis-newmehull[hullR].axis;                   \
                  newmevel->axis = 0;                                                                      \
              }

            if( 0 ) ;
            ELSE_IF_HIT_THEN_MOVE_STOP(x  ,y-1,z  ,0,1,y,>=)
            else if( flags & CBF_PLAT ) ;
            // skip other sides for plat
            ELSE_IF_HIT_THEN_MOVE_STOP(x-1,y  ,z  ,0,1,x,>=)
            ELSE_IF_HIT_THEN_MOVE_STOP(x  ,y  ,z-1,0,1,z,>=)
            ELSE_IF_HIT_THEN_MOVE_STOP(x  ,y  ,z+1,1,0,z,<=)
            ELSE_IF_HIT_THEN_MOVE_STOP(x+1,y  ,z  ,1,0,x,<=)
            ELSE_IF_HIT_THEN_MOVE_STOP(x  ,y+1,z  ,1,0,y,<=)
            else continue;

            recheck[r%2][i] = 1; //I've moved, so recheck me
          }
        }

        for(j=0;j<(r==0?i:maxobjs);j++) { //find other objs to interact with -- don't need to check all on 1st 2 passes
          if(i==j || !fr[a].objs[i].data || !fr[a].objs[j].data )
            continue;
          if( !HAS(fr[b].objs[j].flags,OBJF_POS|OBJF_VEL|OBJF_HULL) )
            continue;

          OBJ_t *oldyou = fr[a].objs+j, *newyou = fr[b].objs+j;
          V *oldyoupos  = flex(oldyou,pos );
          V *newyoupos  = flex(newyou,pos );
          V *newyouvel  = flex(newyou,vel );
          V *oldyouhull = flex(oldyou,hull);
          V *newyouhull = flex(newyou,hull);

          if( newmepos->x+newmehull[0].x >= newyoupos->x+newyouhull[1].x ||   //we dont collide NOW
              newmepos->x+newmehull[1].x <= newyoupos->x+newyouhull[0].x ||
              newmepos->y+newmehull[0].y >= newyoupos->y+newyouhull[1].y ||
              newmepos->y+newmehull[1].y <= newyoupos->y+newyouhull[0].y ||
              newmepos->z+newmehull[0].z >= newyoupos->z+newyouhull[1].z ||
              newmepos->z+newmehull[1].z <= newyoupos->z+newyouhull[0].z    )
            continue;

          if(        oldyoupos->y+oldyouhull[0].y >= oldmepos->y+oldmehull[1].y     //I was above BEFORE
                  && (newyou->flags&OBJF_PLAT) && (newme->flags&OBJF_CLIP)      ) {
            newmepos->y = newyoupos->y + newyouhull[0].y - newmehull[1].y;
            newmevel->y = newyouvel->y;
            recheck[r%2][i] = 1; //I've moved, so recheck me
          } else if( oldyoupos->y+oldyouhull[1].y <= oldmepos->y+oldmehull[0].y     //You were above BEFORE
                  && (newme->flags&OBJF_PLAT) && (newyou->flags&OBJF_CLIP)      ) {
            newyoupos->y = newmepos->y + newmehull[0].y - newyouhull[1].y;
            newyouvel->y = newmevel->y;
            recheck[r%2][j] = 1; //you've moved, so recheck you
          }
        }
      }
    }
    Uint32 adv_game_start = SDL_GetTicks();
    adv_collide_time += adv_game_start - adv_collide_start;
    for(i=0;i<maxobjs;i++) { //mod pass
      OBJ_t *oa = fr[a].objs+i;
      OBJ_t *ob = fr[b].objs+i;
      if(ob->type)
        mod_adv(i,a,b,oa,ob);
    }
    adv_game_time += SDL_GetTicks() - adv_game_start;
    adv_frames++;
    setsurefr(hotfr>50 ? hotfr-50 : 0); //FIXME: UGLY HACK! surefr should be determined for REAL
  }
}


void cleanup()
{
  int i;

  IMG_Quit();
  SDLNet_Quit();
  SDL_Quit();
  clearframebuffer();

  for(i=0;i<maxframes;i++) {
    free(fr[i].cmds);
    free(fr[i].objs);
  }

  free(fr);
  exit(0);
}


// find a free obj slot in frame frame1
// frame is "free" as long as it was empty in the previous frame
// staticly remembers which frames have already been given out this way
// if a slot in the current frame is filled by other code it may be CORRUPTED BY USING THIS
int findfreeslot(int frame1)
{
  static int last_slot = 0;
  static int last_frame = 0;
  int frame0 = (frame1>0)?(frame1-1):(maxframes-1);

  if( last_frame!=frame1 ) {
    last_frame = frame1;
    last_slot = 1;
  }

  if( frame1==-1 ) //exit early
    return -1;

  while(last_slot<maxobjs) {
    if( fr[frame0].objs[last_slot].type==0 &&
        fr[frame1].objs[last_slot].type==0 ) //empty
      return last_slot++;
    last_slot++;
  }

  return -1; //FIXME increase maxobjs instead -- other code will fail until then
}

// clears all objects and commands out of frame buffer
void clearframebuffer()
{
  int i,j;

  for(i=0;i<maxframes;i++) {
    fr[i].dirty = 0;
    memset(fr[i].cmds,0,sizeof(FCMD_t)*maxclients);

    for(j=0;j<maxobjs;j++) {
      if( fr[i].objs[j].data )
        free( fr[i].objs[j].data );
      memset( fr[i].objs+j, 0, sizeof(OBJ_t) );
    }
  }
}


//frame setters
void setmetafr( Uint32 to) {
  metafr = to;
}
void setsurefr( Uint32 to) {
  surefr = to;
}
void setdrawnfr(Uint32 to) {
  drawnfr = to;
}
void sethotfr(  Uint32 to) {
  hotfr = to;
}
void setcmdfr(  Uint32 to) {
  while(cmdfr<to) {
    cmdfr++;
    memset(fr[cmdfr%maxframes].cmds,0,sizeof(FCMD_t)*maxclients);
    fr[cmdfr%maxframes].dirty = 0;
  }
  if(hotfr>=cmdfr)
    hotfr = cmdfr-1;
  if(surefr>=cmdfr)
    SJC_Write("*** DESYNC: cmdfr has been set = or before surefr! ***");
}
void jogframebuffer(Uint32  newmetafr,Uint32 newsurefr) {
  metafr = newmetafr;
  frameoffset = metafr - ticks/ticksaframe;
  surefr  = newsurefr;
  drawnfr = newsurefr;
  hotfr   = newsurefr;
  cmdfr   = newsurefr;
}


