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

//FIXME REMOVE! force amigo to flykick
int flykick = 0;
//

draw_object_sig(amigo)
{
        typedef struct { int x, y, w, h, dx, dy; } xspr;
        xspr tip = {0,0,0,0,0,0}; // extra sprite for tip of sword
        amigo *ami = o->data;

        int w = 50;
        int h = 50;
        int x = 0, y = 0;
        int z = ami->pos.y + ami->hull[1].y;
        switch( ami->state )
        {
                case AMIGO_HELLO:
                        x = (ami->statetime/30) * 50; //                       x   y  w  h  dx  dy
                        if(      ami->statetime<30 ) { x =   0; tip = (xspr){  0,  0, 0, 0,  0,  0}; }
                        else if( ami->statetime<60 ) { x =  50; tip = (xspr){  0,  0, 0, 0,  0,  0}; }
                        else if( ami->statetime<90 ) { x = 100; tip = (xspr){  0,  0, 0, 0,  0,  0}; }
                        else                         { x = 150; tip = (xspr){220,230,10,10,  8,-10}; }
                        y = 150;

                        break;
                case AMIGO_COOLDOWN:
                        tip = (xspr){210,230,10,10, 20,-10};
                        break;

                case AMIGO_JUMP:
                        tip = (xspr){240,230,10,10, 33,-10};
                        x = 0;
                        y = 50;
                        break;

                case AMIGO_SLASH: //                                    x   y  w  h  dx  dy
                        if(      ami->statetime<20 ) { x =  50; tip = (xspr){220,240,30,10,-30, 42}; }
                        else if( ami->statetime<25 ) { x = 100; tip = (xspr){220,230,10,10,  6,-10}; }
                        else                         { x = 150; tip = (xspr){230,230,10,10, 32,-10}; }
                        break;

                case AMIGO_FLYKICK:
                        if( ami->statetime > 30 && ami->sword_dist.x < 80.0f && ami->sword_dist.y < 60.0f )
                                // in this state tip is amigo's left arm
                                //             x   y  w  h  dx  dy
                                tip = (xspr){170,220,20,20, 35, 20};
                        else
                                tip = (xspr){190,220,20,20, 25, 24};

                        x = ((ami->hatcounter%100)/50 ? 100 : 50);
                        y = 50;
                        z += 32;
                        break;

                case AMIGO_DASH:
                        tip = (xspr){210,250,40, 6,-40, 40};
                        x = 150;
                        y = 50;
                        break;
        }
        SJGL_SetTex( sys_tex[TEX_AMIGO].num );
        int c = ami->pos.x;
        int d = ami->pos.y;
        SJGL_Blit(&(REC){     x,     y,     w,     h }, c-34,        d-32,        z);
        SJGL_Blit(&(REC){ tip.x, tip.y, tip.w, tip.h }, c-34+tip.dx, d-32+tip.dy, z);
}

advance_object_sig(amigo)
{
        amigo    *ami     = ob->data;
        float     gravity = 0.6f;

        //FIXME REMOVE! Wrap amigo since he can mostly only go left
        context *co = fr[b].objs[ob->context].data;
        float cowidth = co->x*co->bsx;
        if( ami->pos.x <        -20.0f ) ami->pos.x += cowidth+39.0f;
        if( ami->pos.x > cowidth+20.0f ) ami->pos.x -= cowidth+39.0f;
        //

        spatt(hotfr); // FIXME: probably a BUG
        switch( ami->state )
        {
        case AMIGO_HELLO:
                if( ami->statetime>120 )
                {
                        ami->state = AMIGO_COOLDOWN;
                        ami->statetime = 0;
                }
                break;

        case AMIGO_COOLDOWN:
                if( ami->vel.y!=0 )
                        break;

                ami->vel.x = 0.0f;
                if( ami->statetime>30 ) // decide which attack to do!
                {
                        ami->statetime = 0;
                        switch( patt()%8 )
                        {
                        case 0: ami->state = AMIGO_JUMP;    ami->vel.y -= 10.0f;                     break;
                        case 1: ami->state = AMIGO_JUMP;    ami->vel.y -= 10.0f; ami->vel.x =  4.0f; break;
                        case 2: ami->state = AMIGO_JUMP;    ami->vel.y -= 10.0f; ami->vel.x = -4.0f; break;
                        case 3: ami->state = AMIGO_JUMP;    ami->vel.y -=  8.0f; ami->vel.x =  4.0f; break;
                        case 4: ami->state = AMIGO_JUMP;    ami->vel.y -=  8.0f; ami->vel.x = -4.0f; break;
                        case 5: ami->state = AMIGO_SLASH;                        ami->vel.x = -0.1f; break;
                        case 6: ami->state = AMIGO_DASH;                         ami->vel.x = -7.6f; break;
                        case 7: ami->state = AMIGO_FLYKICK; ami->vel.y  = -3.0f; ami->vel.x = -7.5f;
                                ami->hatcounter = 0;                                                     break;
                        }
                }
                break;

        case AMIGO_JUMP:
                if( ami->statetime>20 )
                {
                        ami->state = AMIGO_COOLDOWN;
                        ami->statetime = 0;
                }
                break;

        case AMIGO_SLASH:
                ami->vel.x += 0.05f;
                if( ami->vel.x > 0.0f )
                        ami->vel.x = 0.0f;
                if( ami->statetime>30 )
                {
                        ami->state = AMIGO_COOLDOWN;
                        ami->statetime = 0;
                }
                break;

        case AMIGO_FLYKICK:
        {
                if( ami->statetime==1 )
                {
                        amigosword *sw = mkamigosword(&ami->sword, ob->context, b, OBJF_POS|OBJF_VEL|OBJF_VIS );
                        if( !sw )
                        {
                                echo("FLYKICK failed, no slot for sword");
                                ami->state = AMIGO_HELLO;
                                break;
                        }
                        sw->pos = ami->pos;
                        sw->vel = (V){1.5f,-2.5f,0.0f};
                        sw->hull[0] = (V){0,0,0};
                        sw->hull[1] = (V){0,0,0};
                        sw->model = 0;
                        sw->owner = objid;
                        sw->spincounter = 0;
                }

                gravity = 0.0f;
                ami->vel.y = 0.0f;
                ami->hatcounter += fabsf(ami->vel.x)*10;
                ami->vel.x += ami->vel.x < -2.0f ? 0.1f : 0.05;
                if( ami->vel.x > 0.0f )
                        ami->vel.x = 0.0f;

                getobject(sw, amigosword, ami->sword);
                ami->sword_dist = (V){ fabsf(sw->pos.x - ami->pos.x), fabsf(sw->pos.y - ami->pos.y), 0 };

                if( ami->statetime>90 )
                {
                        if( ami->sword_dist.x < 41.0f && ami->sword_dist.y < 11.0f )
                        {
                                ami->state = AMIGO_COOLDOWN;
                                ami->statetime = 0;
                                fr[b].objs[ami->sword].flags |= OBJF_DEL;
                        }
                }
                break;
        }

        case AMIGO_DASH:
                ami->vel.x += 0.01f;
                if( ami->vel.y==0.0f && fabsf(ami->vel.x)>2.0f && !(patt()%60) )
                        ami->pos.y -= (patt()%2+2)*2.3f; // turbulence on the ground

                if( ami->statetime>50 )
                {
                        ami->state = AMIGO_COOLDOWN;
                        ami->statetime = 0;
                }
                break;
        }

        ami->statetime++;
        ami->vel.y += gravity;
}
