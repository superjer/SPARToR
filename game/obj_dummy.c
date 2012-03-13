/**
 **  McDiddy's: The Game: Vengeance
 **  Implementation example for the SPARToR Network Game Engine
 **  Copyright (c) 2010-2012  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "obj_.h"

void obj_dummy_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co )
{
  DUMMY_t *du = o->data;
  int c = POINT2NATIVE_X(&du->pos);
  int d = POINT2NATIVE_Y(&du->pos);

  sprblit( &SM(tnl_weight_silver), c, d );
}

void obj_dummy_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  DUMMY_t *du = ob->data;

  // friction
  if(      du->vel.x> 0.1f ) du->vel.x -= 0.1f;
  else if( du->vel.x>-0.1f ) du->vel.x  = 0.0f;
  else                       du->vel.x += 0.1f;

  if( objid==(int)(hotfr+100)%2000 ) //tee-hee
    du->vel.x += (float)(b%4)-1.5;

  du->vel.y += 0.7f;        //gravity
}

