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

PROTO_DRAW(AMIGOSWORD)
{
        AMIGOSWORD_t *sw = o->data;
        int posx = sw->pos.x;
        int posy = sw->pos.y;
        SJGL_SetTex( sys_tex[TEX_AMIGO].num );
        SJGL_Blit( &(REC){ 200, 50+50*(hotfr%3), 56, 50 }, posx-25, posy-28, sw->pos.y );
}

PROTO_ADVANCE(AMIGOSWORD)
{
        AMIGOSWORD_t *sw = ob->data;
        sw->spincounter++;
        if( sw->spincounter > 45 )
        {
                AMIGO_t *am = fr[b].objs[sw->owner].data;
                sw->vel.x = am->pos.x+40.0f - sw->pos.x;
                sw->vel.y = am->pos.y       - sw->pos.y;
                float normalize = sqrt(sw->vel.x * sw->vel.x + sw->vel.y * sw->vel.y);
                if( normalize > 4.0f )
                {
                        normalize = 4.0f / normalize;
                        sw->vel.x *= normalize;
                        sw->vel.y *= normalize;
                }
        }
}
