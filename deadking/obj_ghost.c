/**
 **  Dead Kings' Quest
 **  A special game for the SPARToR Network Game Engine
 **  Copyright (c) 2010-2015  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "mod.h"
#include "saveload.h"
#include "sprite.h"
#include "sprite_helpers.h"
#include "saveload.h"

static void ghost_paint( FCMD_t *c, ghost *gh, context *co );

draw_object_sig(ghost)
{
        if( !v_drawhulls ) return;

        // draw ghost (optional)
        //ghost *gh = o->data;
}

advance_object_sig(ghost)
{
        ghost   *gh = ob->data;
        context *co = fr[b].objs[ob->context].data;

        static V dimetric_hull[2] = {{-64, 0, -64}, {64, 72, 64}};
        static V ortho_hull[2]    = {{-NATIVEW/2, -NATIVEH/2, 0}, {NATIVEW/2, NATIVEH/2, 0}};

        if( co->projection == DIMETRIC     )
                memcpy( gh->hull, dimetric_hull, sizeof (V[2]) );
        if( co->projection == ORTHOGRAPHIC )
                memcpy( gh->hull, ortho_hull,    sizeof (V[2]) );

        if( gh->client==me )
        {
                myghost     = objid;
                mycontext   = ob->context;

                v_camx = gh->pos.x;
                v_camy = gh->pos.y;

                v_targx = gh->pos.x;
                v_targy = gh->pos.y;
                v_targz = gh->pos.z;
        }

        switch( fr[b].cmds[gh->client].cmd )
        {
                case CMDT_1CAMLEFT:  gh->goingl = 1; break;
                case CMDT_0CAMLEFT:  gh->goingl = 0; break;
                case CMDT_1CAMRIGHT: gh->goingr = 1; break;
                case CMDT_0CAMRIGHT: gh->goingr = 0; break;
                case CMDT_1CAMUP:    gh->goingu = 1; break;
                case CMDT_0CAMUP:    gh->goingu = 0; break;
                case CMDT_1CAMDOWN:  gh->goingd = 1; break;
                case CMDT_0CAMDOWN:  gh->goingd = 0; break;
        }

        gh->vel = (V){0, 0, 0};

        if( gh->goingr || gh->goingu ) gh->vel.x += -5;
        if( gh->goingl || gh->goingd ) gh->vel.x +=  5;
        if( gh->goingr || gh->goingd ) gh->vel.z +=  5;
        if( gh->goingl || gh->goingu ) gh->vel.z += -5;

        FCMD_t *c = fr[b].cmds + gh->client;

        switch( c->cmd )
        {
                case CMDT_0CON: //FIXME: edit rights!
                {
                        size_t n = 0;
                        char letter = (char)unpackbytes(c->data, MAXCMDDATA, &n, 1);

                        switch( letter )
                        {
                                case 'o': // orthographic
                                        push_context(co);
                                        co->projection = ORTHOGRAPHIC;
                                        echo("Setting context projection to ORTHOGRAPHIC");
                                        break;

                                case 'd': // dimetric
                                        push_context(co);
                                        co->projection = DIMETRIC;
                                        echo("Setting context projection to DIMETRIC");
                                        break;

                                case 'b': { // bounds
                                        push_context(co);
                                        int x = (int)unpackbytes(c->data, MAXCMDDATA, &n, 4);
                                        int y = (int)unpackbytes(c->data, MAXCMDDATA, &n, 4);
                                        int z = (int)unpackbytes(c->data, MAXCMDDATA, &n, 4);

                                        context tmp;
                                        const char *error = create_context(&tmp, co, x, y, z);

                                        if( error )
                                                echo("%s", error);
                                        else
                                                memcpy(co, &tmp, sizeof tmp);

                                        break; }

                                case 'z': { // blocksize
                                        push_context(co);
                                        co->bsx = (int)unpackbytes(c->data, MAXCMDDATA, &n, 4);
                                        co->bsy = (int)unpackbytes(c->data, MAXCMDDATA, &n, 4);
                                        co->bsz = (int)unpackbytes(c->data, MAXCMDDATA, &n, 4);

                                        break; }

                                case 't': { // tilespacing
                                        push_context(co);
                                        co->tileuw = (int)unpackbytes(c->data, MAXCMDDATA, &n, 4);
                                        co->tileuh = (int)unpackbytes(c->data, MAXCMDDATA, &n, 4);

                                        break; }

                                default:
                                        echo("Unknown edit command!");
                                        break;
                        }

                        // do NOT free co->map, co->dmap, it will get GC'd as it rolls off the buffer! really!
                        break;
                }

                case CMDT_0EUNDO:
                        pop_context(co);
                        break;

                case CMDT_0EPAINT: //FIXME: UNSAFE check for edit rights, data values
                        ghost_paint( c, gh, co );
                        break;
        }
}

static void ghost_paint( FCMD_t *c, ghost *gh, context *co )
{
        size_t n = 0;
        int  i, j, k;
        char letter = (char)unpackbytes(c->data, MAXCMDDATA, &n, 1);
        int  dnx    = (int) unpack(4);
        int  dny    = (int) unpack(4);
        int  dnz    = (int) unpack(4);
        int  upx    = (int) unpack(4);
        int  upy    = (int) unpack(4);
        int  upz    = (int) unpack(4);
        int  sprnum = (int) unpack(4);

        if( letter!='p' ) { echo("Unknown edit command!"); return; }

        int tool_num = (sprites[sprnum].flags & TOOL_MASK);

        int shx = 0;
        int shy = 0;
        int shz = 0;

        int clipx = MAX(gh->clipboard_x, 1);
        int clipy = MAX(gh->clipboard_y, 1);
        int clipz = MAX(gh->clipboard_z, 1);

        //make so dn is less than up... also adjust clipboard shift
        if( dnx > upx ) { SWAP(upx, dnx, int); shx = clipx-(upx-dnx+1)%clipx; }
        if( dny > upy ) { SWAP(upy, dny, int); shy = clipy-(upy-dny+1)%clipy; }
        if( dnz > upz ) { SWAP(upz, dnz, int); shz = clipz-(upz-dnz+1)%clipz; }

        if( dnx<0 || dny<0 || dnz<0 || upx>=co->x || upy>=co->y || upz>=co->z )
        {
                echo("Paint command out of bounds!");
                return;
        }

        if( tool_num == TOOL_COPY ) //COPY tool texture
        {
                gh->clipboard_x = clipx = upx - dnx + 1;
                gh->clipboard_y = clipy = upy - dny + 1;
                gh->clipboard_z = clipz = upz - dnz + 1;
                gh->clipboard_data = malloc( clipx*clipy*clipz*(sizeof *gh->clipboard_data) ); //FIXME: mem leak
        }

        if( tool_num == TOOL_PSTE && !gh->clipboard_data ) { echo("Clipboard is empty"); return; }

        push_context(co);

        for( k=dnz; k<=upz; k++ ) for( j=dny; j<=upy; j++ ) for( i=dnx; i<=upx; i++ )
        {
                int pos = k*co->y*co->x + j*co->x + i;

                if( !tool_num ) // regular tile painting
                {
                        int dsprnum = sprnum;

                        if( co->projection == ORTHOGRAPHIC )
                                dsprnum = sprite_grid_transform_xy(sprites + sprnum, co, i, j, k, i-dnx, j-dny, upx-dnx+1, upy-dny+1) - sprites;

                        co->dmap[pos].flags &= ~CBF_NULL;
                        co->dmap[pos].flags |= CBF_VIS;
                        co->dmap[pos].spr    = dsprnum;
                        continue;
                }

                switch( tool_num )
                {
                        case TOOL_NUL:
                                co->dmap[pos] = co->map[pos];
                                co->dmap[pos].flags |= CBF_NULL;
                                break;

                        case TOOL_SOL:
                                co->dmap[pos].flags &= ~(CBF_NULL|CBF_PLAT);
                                co->dmap[pos].flags |= CBF_SOLID;
                                break;

                        case TOOL_PLAT:
                                co->dmap[pos].flags &= ~(CBF_NULL|CBF_SOLID);
                                co->dmap[pos].flags |= CBF_PLAT;
                                break;

                        case TOOL_OPN:
                                co->dmap[pos].flags &= ~(CBF_NULL|CBF_SOLID|CBF_PLAT);
                                break;

                        case TOOL_COPY:
                                gh->clipboard_data[ (k-dnz)*clipy*clipx + (j-dny)*clipx + (i-dnx) ] = co->dmap[pos];
                                break;

                        case TOOL_PSTE:
                        {
                                int x = (i-dnx+shx) % clipx;
                                int y = (j-dny+shy) % clipy;
                                int z = (k-dnz+shz) % clipz;
                                co->dmap[pos] = gh->clipboard_data[ x + y*clipx + z*clipy*clipx ];
                                break;
                        }

                        case TOOL_OBJ:
                                // disabled, PLAYER_T removed
                                break;

                        case TOOL_ERAS:
                                co->dmap[pos].flags &= ~(CBF_VIS|CBF_NULL);
                                break;

                        case TOOL_VIS:
                                co->map[pos].flags |= CBF_VIS; // hack for making a loaded-from-file tile visible (format change mess)
                                break;
                }
        }
}
