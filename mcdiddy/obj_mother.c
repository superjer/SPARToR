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

void mk_ghost_and_player(int client, int context, unsigned int frame)
{
        int mostflags = OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_BNDX|OBJF_BNDZ|OBJF_BNDB;
        int ghostslot;
        int playerslot;

        //FIXME context is hardcoded as 1 for GHOST and PLAYER:
        ghost *gh = mkghost(&ghostslot, context, frame, mostflags|OBJF_BNDT);
        player *pl = mkplayer(&playerslot, context, frame, mostflags|OBJF_PVEL|OBJF_PLAT|OBJF_CLIP);

        if( !gh || !pl )
        {
                echo("%d: Failed to create ghost and player for client %i", frame, client);
                return;
        }

        debug( "%d: New client %i created ghost is obj#%d player is obj#%d", frame, client, ghostslot, playerslot );

        gh->client         = client;
        gh->avatar         = playerslot;
        gh->clipboard_data = NULL;

        pl->pos            = (V){ 0,-50, 0};
        pl->hull[0]        = (V){-6,-30,-6};
        pl->hull[1]        = (V){ 6,  0, 6};
        pl->model          = client%5;
        pl->ghost          = ghostslot;
        pl->facingr        = 1;
}

draw_object_sig(mother)
{
}

advance_object_sig(mother)
{
        int i, j;

        for( i=0; i<maxclients; i++ )
        {
                if( !(fr[b].cmds[i].flags & CMDF_NEW) )
                        continue;

                for( j=0; j<maxobjs; j++ )
                        if( fr[b].objs[j].type == ghost_type && ((ghost *)fr[b].objs[j].data)->client==i )
                                echo( "%d: Client %i already has a ghost at obj#%d!", hotfr, i, j );
                
                mk_ghost_and_player(i, 1, b);
        }

        //create a slug every now and then
        if (hotfr % 17 == 0)
        {
                slug *sl = mkslug(NULL, 1, b,
                                 OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDB|OBJF_BNDZ);

                if (sl)
                {
                        sl->pos     = (V){(hotfr % 2) * 368 + 8, 0, 0};
                        sl->vel     = (V){(hotfr % 2) ? -0.25f : 0.25f, 0, 0};
                        sl->hull[0] = (V){-8,-12,-8};
                        sl->hull[1] = (V){ 8,  0, 8};
                        sl->model   = 0;
                        sl->dead    = 0;
                }
        }

        //create AMIGO!
        if (hotfr == 200)
        {
                amigo *ami = mkamigo(NULL, 1, b,
                                   OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDB|OBJF_BNDZ);

                if (ami)
                {
                        ami->pos       = (V){16*148,16*5,0};
                        ami->vel       = (V){0,0,0};
                        ami->hull[0]   = (V){-8,-18,-8};
                        ami->hull[1]   = (V){ 8, 18, 8};
                        ami->model     = 0;
                        ami->state     = AMIGO_HELLO;
                        ami->statetime = 0;
                }
        }
}
