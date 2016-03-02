#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"
#include "main.h"

void draw_guides(context *co, int x, int y, int z);
