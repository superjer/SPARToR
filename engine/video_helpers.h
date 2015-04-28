#ifdef DOGLEW
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include "SDL.h"
#include "main.h"

void draw_guides(CONTEXT_t *co, int x, int y, int z);
