#include "mod.h"
#include "audio.h"

void real_function(FRAME_t *fr, int maxobjs, int objid, unsigned int a, unsigned int b, object *oa, object *ob)
{
        int i;
        slug    *sl = ob->data;
        context *co = fr[b].objs[ob->context].data;
        int kill = 0;
        sl->vel.y += 0.3;      //gravity

        if( sl->dead )          //decay
        {
                sl->dead++;
        }
        else for( i=0;i<maxobjs;i++ ) //find players, bullets to hit
        {
                if( fr[b].objs[i].type == player_type )
                {
                        player *pl = fr[b].objs[i].data;

                        int up_stabbed = pl->stabbing<0
                                && fabsf(sl->pos.x                 - pl->pos.x                )<=14.0f
                                && fabsf(sl->pos.y + sl->hull[1].y - pl->pos.y - pl->hull[0].y)<=8.0f ;

                        int dn_stabbed = pl->stabbing>0
                                && fabsf(sl->pos.x                 - pl->pos.x                )<=14.0f
                                && fabsf(sl->pos.y + sl->hull[0].y - pl->pos.y - pl->hull[1].y)<=4.0f ;

                        if( up_stabbed )
                        {
                                pl->vel.y = sl->vel.y;
                                sl->vel.y = -2.5f;
                                kill = 1;
                                //play("stab"); FIXME add this back in!
                        }
                        else if( dn_stabbed )
                        {
                                pl->vel.y = -2.25f;
                                pl->hovertime = 14;
                                sl->vel.y = 0.0f;
                                kill = 1;
                                //play("stab"); FIXME add this back in!
                        }
                }
                else if( fr[b].objs[i].type == bullet_type )
                {
                        bullet *bu = fr[b].objs[i].data;
                        if( fabsf(sl->pos.x - bu->pos.x)>8.0f || fabsf(sl->pos.y - bu->pos.y)>8.0f )
                                continue; // no hit
                        bu->ttl = 0;
                        sl->vel.y = -1.5f;
                        kill = 1;
                        //play("wibbity");
                }
        }

        if( kill )
        {
                sl->vel.x /= 100.0f; //preserve direction while dead
                sl->dead = 1;
                ob->flags &= ~OBJF_PLAT;
        }

        // slug stops clipping after 5th frame of death
        if( sl->dead==5 )
                ob->flags &= ~(OBJF_CLIP|OBJF_BNDB);

        // delete the slug if it's gone out-of-bounds, or is too dead, or has stopped
        if(    sl->dead > 100 || sl->vel.x == 0 || sl->pos.x < -10.0f
            || sl->pos.x > co->x*co->bsx+10.0f
            || sl->pos.y > co->y*co->bsy+10.0f )
                ob->flags |= OBJF_DEL;
}
