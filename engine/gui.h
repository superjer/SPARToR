
#ifndef SPARTOR_GUI_H_
#define SPARTOR_GUI_H_

#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"
#include "SDL_net.h"
#include "main.h"
#include "video.h"
#include "input.h"

// globals
extern int gui_hover;

// prototypes
int gui_element_at( Uint32 guifr, int x, int y );
void gui_update( Uint32 guifr );

#endif
