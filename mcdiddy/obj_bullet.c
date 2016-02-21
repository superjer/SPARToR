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

PROTO_DRAW(BULLET)
{
        BULLET_t *bu = o->data;
        int g = bu->pos.x;
        int h = bu->pos.y;
        SJGL_SetTex( sys_tex[TEX_PLAYER].num );
        SJGL_Blit( &(REC){144,150,4,4}, g-2, h-2, NATIVEH );
}

PROTO_ADVANCE(BULLET)
{
        int i;
        BULLET_t  *bu = ob->data;
        BULLET_t  *oldbu = oa->data;
        CONTEXT_t *co = fr[b].objs[ob->context].data;

        if( !oldbu ) return;

        if( bu->ttl ) bu->ttl--;

        for( i=0; i<maxobjs; i++ )  //find players to hit
                if( fr[b].objs[i].type==OBJT_PLAYER )
                {
                        PLAYER_t *pl = fr[b].objs[i].data;
                        if( i==bu->owner                       || //player owns bullet
                            fabsf(bu->pos.x - pl->pos.x)>10.0f || //not touching
                            fabsf(bu->pos.y - pl->pos.y)>15.0f    )
                                continue;
                        pl->vel.y += -5.0f;
                        pl->vel.x += (bu->vel.x>0.0f?5.0f:-5.0f);
                        bu->dead = 1;
                        bu->ttl = 0; //delete bullet
                }

        if( (bu->ttl==0 || bu->pos.y>co->y*co->bsy) && !bu->dead )
        {
                bu->dead = 1;
                bu->ttl = 90;
                bu->hull[0] = (V){-1,-1,-1};
                bu->hull[1] = (V){ 1, 1, 1};
                ob->flags |= OBJF_CLIP|OBJF_HULL|OBJF_BNDB;
        }

        if( bu->pos.x<=-10.0f || bu->pos.x>=co->x*co->bsx+10.0f || bu->ttl==0 )
        {
                bu->dead = 1;
                ob->flags |= OBJF_DEL;
        }

        if( bu->dead ) // gravity, friction only if dead
        {
                if( !oldbu->dead && fr[b].objs[bu->owner].type==OBJT_PLAYER )
                        ((PLAYER_t *)fr[b].objs[bu->owner].data)->projectiles--;

                bu->pos.z =  1.0f;

                if( oldbu->dead && bu->vel.y==0.0f )
                        bu->vel.y = -0.4f * oldbu->vel.y;
                else
                        bu->vel.y += 0.50001f;

                if( fabsf(bu->vel.x)<0.1f ) bu->vel.x = 0.0f;
                else                        bu->vel.x *= bu->vel.y ? 0.95f : 0.90f;
        }
}
