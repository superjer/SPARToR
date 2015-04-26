//
// This is the header for all obj_*.c files
//

#ifndef MOD_OBJ_H_
#define MOD_OBJ_H_

#include "mod.h"


void obj_mother_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );

void obj_ghost_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_ghost_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );

void obj_dummy_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_dummy_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );

void obj_player_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_player_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );

void obj_person_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_person_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );

void obj_bullet_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_bullet_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );

void obj_slug_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_slug_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );

void obj_amigo_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_amigo_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );

void obj_amigosword_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co );
void obj_amigosword_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob );

#endif

