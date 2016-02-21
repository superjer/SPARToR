/**
 **  McDiddy's: The Game: Vengeance
 **  Implementation example for the SPARToR Network Game Engine
 **  Copyright (c) 2010-2015  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "mod.h"

PROTO_DRAW(DUMMY)
{
        DUMMY_t *du = o->data;
        sprblit( &SM(tnl_weight_silver), du->pos.x, du->pos.y );
}

PROTO_ADVANCE(DUMMY)
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
