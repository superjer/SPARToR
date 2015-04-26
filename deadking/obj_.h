//
// This is the header for all obj_*.c files
//

#ifndef DEADKING_OBJ_H_
#define DEADKING_OBJ_H_

#include "mod.h"


void obj_mother_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );

void obj_ghost_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_ghost_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );

void obj_person_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_person_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );

#endif

