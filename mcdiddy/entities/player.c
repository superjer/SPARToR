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
#include "sprite.h"
#include "audio.h"
#include "helpers.h"

//FIXME REMOVE! change local player model
int    setmodel = -1;
//

draw_object_sig(player)
{
        player *pl = o->data;
        int c = pl->pos.x;
        int d = pl->pos.y;

        //girl hair
        if     ( pl->model!=4 ) ;
        else if( pl->facingr  ) sprblit( &SM(girlhair_r), c, d-30+(pl->goingd?4:0)+pl->gundown/7 );
        else                    sprblit( &SM(girlhair_l), c, d-30+(pl->goingd?4:0)+pl->gundown/7 );

        //player sprite
        if( pl->goingd )
        {
                if     ( pl->turning ) sprblit( &SM(ctblue_duck_f), c, d );
                else if( pl->facingr ) sprblit( &SM(ctblue_duck_r), c, d );
                else                   sprblit( &SM(ctblue_duck_l), c, d );
        }
        else
        {
                if     ( pl->turning ) sprblit( &SM(ctblue_f),      c, d );
                else if( pl->facingr ) sprblit( &SM(ctblue_r),      c, d );
                else                   sprblit( &SM(ctblue_l),      c, d );
        }

        // knife or gun
        if     ( pl->stabbing<0 ) sprblit( &SM(knife_up),   c, d-44 );
        else if( pl->stabbing>0 ) sprblit( &SM(knife_down), c, d+10 );
        else
        {
                int gb = pl->gunback;
                int gd = pl->gundown/5;

                if     ( pl->goingu && pl->facingr ) sprblit( &SM(mp5_up_r),   c+20-gb, d-25+gd );
                else if( pl->goingu                ) sprblit( &SM(mp5_up_l),   c-20+gb, d-25+gd );
                else if( pl->goingd && pl->facingr ) sprblit( &SM(mp5_down_r), c+20-gb, d-10+gd );
                else if( pl->goingd                ) sprblit( &SM(mp5_down_l), c-20+gb, d-10+gd );
                else if(               pl->facingr ) sprblit( &SM(mp5_r),      c+20-gb, d-19+gd );
                else                                 sprblit( &SM(mp5_l),      c-20+gb, d-19+gd );
        }
}

void create_bullet(unsigned int objid, unsigned int b, object *ob)
{
        player *pl = ob->data;
        bullet *bu = mkbullet(NULL, ob->context, b, OBJF_POS|OBJF_VEL|OBJF_VIS);
        if( !bu ) return;

        play("mp5_shot");

        if( pl->facingr )
        {
                bu->pos = (V){pl->pos.x+19,pl->pos.y-19,pl->pos.z};
                bu->vel = (V){ 3,0,0};
        }
        else
        {
                bu->pos = (V){pl->pos.x-19,pl->pos.y-19,pl->pos.z};
                bu->vel = (V){-3,0,0};
        }

        if( pl->goingu ) // aiming
        {
                bu->vel.y += -4;
                bu->pos.y += -6;
        }

        if( pl->goingd )
        {
                bu->vel.y +=  4;
                bu->pos.y += 10;
        }

        bu->model       = 1;
        bu->owner       = objid;
        bu->ttl         = 100;
        pl->cooldown = 5;
        pl->gunback  = 2;
        pl->projectiles++;
}

advance_object_sig(player)
{
        int i;
        player *oldme = oa->data;
        player *newme = ob->data;
        ghost *gh = fr[b].objs[newme->ghost].data;

        switch( fr[b].cmds[gh->client].cmd )
        {
                case CMDT_1LEFT:  newme->goingl  = 1;
                          if(  newme->facingr ) newme->turning = 3;
                                  newme->facingr = 0;                      break;
                case CMDT_0LEFT:  newme->goingl  = 0;                      break;
                case CMDT_1RIGHT: newme->goingr  = 1;
                          if( !newme->facingr ) newme->turning = 3;
                                  newme->facingr = 1;                      break;
                case CMDT_0RIGHT: newme->goingr  = 0;                      break;
                case CMDT_1UP:    newme->goingu  = 1;                      break;
                case CMDT_0UP:    newme->goingu  = 0;                      break;
                case CMDT_1DOWN:  newme->goingd  = 1;                      break;
                case CMDT_0DOWN:  newme->goingd  = 0;                      break;
                case CMDT_1JUMP:  newme->jumping = 1;                      break;
                case CMDT_0JUMP:  newme->jumping = 0;                      break;
                case CMDT_1FIRE:  newme->firing  = 1;                      break;
                case CMDT_0FIRE:  newme->firing  = 0; newme->cooldown = 0; break;
        }

        if( !oldme ) //FIXME why's this null?
        {
                debug("Warning: oldme is NULL!");
                return;
        }

        gh->vel.x = newme->pos.x - gh->pos.x; // get ghost heading in right direction
        gh->vel.y = newme->pos.y - gh->pos.y;
        gh->vel.z = newme->pos.z - gh->pos.z;

        if( gh->client==me ) //local client match
        {
                v_camx = gh->pos.x;
                v_camy = gh->pos.y;

                if( setmodel>-1 ) //FIXME -- just for fun, will not sync!
                {
                        newme->model = setmodel;
                        setmodel = -1;
                }
        }

        if( newme->firing || (newme->stabbing<0 && !newme->goingu)
                          || (newme->stabbing>0 && !newme->goingd) ) //firing or stopping pressing up/down
        {
                newme->stabbing = 0;
        }
        else if( !newme->stabbing && newme->vel.y!=0.0 )         //freefalling, not stabbing
        {
                if( newme->goingu && !oldme->goingu )            //just pressed up
                        newme->stabbing = -4;
                if( newme->goingd && !oldme->goingd )            //just pressed down
                        newme->stabbing = 4;
        }

        if( newme->stabbing==-1 || newme->stabbing==1 )          //last frame of stabbing is over
                newme->stabbing = 0;

        if( newme->stabbing && newme->vel.y==0.0 )               //tink tink tink!
        {
                if( newme->stabbing>0 )
                {
                        newme->vel.y -= 0.4f*oldme->vel.y;
                        play("tink");
                }
                else
                {
                        play("tink2");
                }

                newme->stabbing += (newme->stabbing>0 ? -1 : 1);
        }

        if( newme->stabbing && newme->goingu )          //expand hull for stabbing
        {
                newme->hull[0].y = -44;
                newme->hull[1].y =   0;
        }
        else if( newme->stabbing && newme->goingd )
        {
                newme->hull[0].y = -30;
                newme->hull[1].y =  10;
        }
        else
        {
                newme->hull[0].y = -30;
                newme->hull[1].y =   0;
        }

        newme->gunback = 0; //reset gun position

        if( newme->goingr || newme->goingl )
                newme->gundown = (newme->gundown+1)%10;
        else
                newme->gundown = 0;

        // friction
        #define P_FRIC(velxz,amt)                                  \
                if(      newme->velxz> amt ) newme->velxz -= amt;  \
                else if( newme->velxz>-amt ) newme->velxz  = 0.0f; \
                else                         newme->velxz += amt;
        P_FRIC( vel.x, 0.1f)
        P_FRIC(pvel.x, 0.25f)
        #undef P_FRIC

        if( newme->turning )
                newme->turning--;

        // -- WALK --
        if( newme->goingl )
                newme->vel.x = newme->vel.x<-1.0f ? -1.0f : newme->vel.x-0.5f;
        if( newme->goingr )
                newme->vel.x = newme->vel.x> 1.0f ?  1.0f : newme->vel.x+0.5f;

        // -- JUMP --
        if( newme->pvel.y <= -1.0f )    //jumping in progress
        {
                newme->pvel.y += 1.0f;        //jumpvel fades into real velocity
                newme->vel.y += -1.0f;
        }
        else if( newme->pvel.y < 0.0f ) //jumping ending
        {
                newme->vel.y += newme->pvel.y;
                newme->pvel.y = 0.0f;
                newme->jumping = 0;           //must press jump again now
        }

        if( !newme->jumping )           //low-jump, cancel jump velocity early
                newme->pvel.y = 0.0f;

        if( (newme->vel.y==0.0f || oldme->vel.y==0.0f) && newme->jumping ) //FIXME 0 velocity means grounded? not really
        {
                newme->pvel.y = -6.3; //initiate jump!
                play("jump2");
        }

        // hit head or land on feet sound effects
        if( newme->vel.y==0.0f )
        {
                if( oldme->vel.y>0.0f && oldme->pos.y < newme->pos.y )
                {
                        play("footstep1");
                        newme->walkcounter = 0;
                }

                if( oldme->vel.y<0.0f )
                        play("headbump");
        }

        // walk sound effects
        if( newme->vel.y==0.0f && fabsf(newme->vel.x)>0.45f )
        {
                newme->walkcounter = (newme->walkcounter+1) % 28;
                if( newme->walkcounter==0 )
                        play("footstep1");
                else if( newme->walkcounter==14 )
                        play("footstep2");
        }

        // -- FIRE --
        if( newme->cooldown>0 )
                newme->cooldown--;

        if( newme->firing && newme->cooldown==0 && newme->projectiles<20 )
                create_bullet(objid, b, ob);

        for( i=0; i<objid; i++ )  //find other players to interact with -- who've already MOVED
                if( fr[b].objs[i].type == player_type )
                {
                        player *oldyou = fr[a].objs[i].data;
                        player *newyou = fr[b].objs[i].data;

                        if(    !oldyou
                            || fabsf(newme->pos.x - newyou->pos.x)>5.0f //we're not on top of each other
                            || fabsf(newme->pos.z - newyou->pos.z)>5.0f
                            || fabsf(newme->pos.y - newyou->pos.y)>2.0f
                            || newme->goingr  ||  newme->goingl         //or we're moving
                            || newyou->goingr || newyou->goingl         )
                                continue;

                        if( newme->pos.x < newyou->pos.x )
                        {
                                newme->pvel.x  -= 0.1f;
                                newyou->pvel.x += 0.1f;
                        }
                        else
                        {
                                newme->pvel.x  += 0.1f;
                                newyou->pvel.x -= 0.1f;
                        }
                }

        if( newme->hovertime ) //gravity?
        {
                newme->hovertime--;
                newme->vel.y += 0.05f;
        }
        else
        {
                newme->vel.y += 0.35f;
        }

        /*
        // LIVE: uncomment to move the player somewhere
        newme->pos.x = 2300;
        newme->pos.y = 100;
        newme->vel.x = 0;
        newme->vel.y = 0;
        */
}
