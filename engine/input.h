
#ifndef SPARTOR_INPUT_H_
#define SPARTOR_INPUT_H_

#include "SDL.h"
#include "SDL_net.h"


#define INP_KEYB 1 //keyboard
#define INP_JBUT 2 //joystick button
#define INP_JAXP 3 //joystick axis, positive
#define INP_JAXN 4 //joystick axis, negative
#define INP_MBUT 5 //mouse button
#define INP_MAX  5 //max device type

extern char *inputdevicenames[];


extern int i_mousex; //last seen mouse position
extern int i_mousey;

extern int i_hasmouse;
extern int i_hasfocus;
extern int i_minimized;

extern int i_watch;

void inputinit();
void putcmd(int device,int sym,int press);
void setactive(Uint8 gain,Uint8 state);
void kbinput(int press,SDL_keysym keysym);
void joyinput(int press,SDL_JoyButtonEvent jbutton);
void axisinput(SDL_JoyAxisEvent jaxis);
void mouseinput(int press,SDL_MouseButtonEvent mbutton);
void mousemove(SDL_MouseMotionEvent mmotion);
void readinput();
void input_bindsoon(int presscmd,int releasecmd);
void kwikbind(int device,int sym);

#endif

