#ifndef SPARTOR_SAVELOAD_H_
#define SPARTOR_SAVELOAD_H_

#include "main.h"

#define MAPVERSION 2


int save_context(const char *name,int context,int savefr);
int load_context(const char *file,int context,int loadfr);
const char *create_context(CONTEXT_t *co, const CONTEXT_t *ref, int x, int y, int z);

#endif
