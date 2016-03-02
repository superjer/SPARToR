#ifndef SPARTOR_SAVELOAD_H_
#define SPARTOR_SAVELOAD_H_

#include "main.h"

#define MAPVERSION 2


int save_context(const char *name, int context, int savefr);
int load_context(const char *file, int context, int loadfr);
const char *create_context(context *co, const context *ref, int x, int y, int z);
void destroy_context(context *co, int really);
void push_context(context *co);
void pop_context(context *co);

#endif
