#ifndef SPARTOR_INPUT_H_
#define SPARTOR_INPUT_H_

#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"
#include "SDL_net.h"

#define INP_KEYB 1 //keyboard
#define INP_JBUT 2 //joystick button
#define INP_JAXP 3 //joystick axis, positive
#define INP_JAXN 4 //joystick axis, negative
#define INP_MBUT 5 //mouse button
#define INP_MAX  5 //max device type

#define MAX_PADS 32 //max number of game controllers

extern char *inputdevicenames[];

extern int i_mousex; //last seen mouse position
extern int i_mousey;

extern int i_hasmouse;
extern int i_hasfocus;
extern int i_minimized;

extern int i_watch;

void inputinit();
void putcmd(int device, int sym, int press);
void winevent(SDL_WindowEvent e);
void textinput(SDL_TextInputEvent e);
void textedit(SDL_TextEditingEvent e);
void kbinput(int press, SDL_KeyboardEvent e);
void padadd(SDL_ControllerDeviceEvent e);
void padremap(SDL_ControllerDeviceEvent e);
void padremove(SDL_ControllerDeviceEvent e);
void padinput(int press, SDL_ControllerButtonEvent e);
void padaxis(SDL_ControllerAxisEvent e);
void mouseinput(int press, SDL_MouseButtonEvent mbutton);
void mousemove(SDL_MouseMotionEvent mmotion);
void readinput();
void input_bindsoon(int presscmd, int releasecmd);
void kwikbind(int device, int sym);

#endif
