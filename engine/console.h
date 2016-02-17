#ifndef SPARTOR_CONSOLE_H_
#define SPARTOR_CONSOLE_H_

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
#include "font.h"

#define echo console_write
#define debug console_debug
#define CONSOLE_BUFLEN 200

void console_init();
void console_put(char c);
void console_replace(const char *c);
void console_write(const char *s, ...);
void console_debug(const char *s, ...);
void console_log(const char *newfile);
void console_rub(int right);
void console_clear();
void console_up();
void console_down();
void console_left();
void console_right();
void console_home();
void console_end();
void console_copy();
void console_paste();
char *console_submit();
char **console_getbuf();
int console_getpos();

#endif
