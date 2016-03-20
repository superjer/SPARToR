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
#include "helpers.h"

draw_object_sig(amigosword)
{
        amigosword *sw = o->data;
        int posx = sw->pos.x;
        int posy = sw->pos.y;
        SJGL_SetTex( sys_tex[TEX_AMIGO].num );
        SJGL_Blit( &(REC){ 200, 50+50*(hotfr%3), 56, 50 }, posx-25, posy-28, sw->pos.y );
}

advance_object_sig(amigosword)
{
        amigosword *sw = ob->data;
        sw->spincounter++;
        if( sw->spincounter > 45 )
        {
                amigo *ami = fr[b].objs[sw->owner].data;
                sw->vel.x = ami->pos.x+40.0f - sw->pos.x;
                sw->vel.y = ami->pos.y       - sw->pos.y;
                float normalize = sqrt(sw->vel.x * sw->vel.x + sw->vel.y * sw->vel.y);
                if( normalize > 4.0f )
                {
                        normalize = 4.0f / normalize;
                        sw->vel.x *= normalize;
                        sw->vel.y *= normalize;
                }
        }
}
