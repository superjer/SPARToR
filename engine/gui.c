/**
 **  SPARToR
 **  Network Game Engine
 **  Copyright (C) 2010-2015  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "gui.h"
#include "console.h"

// globals
int gui_hover = 0;

void gui_update( Uint32 guifr )
{
        /*
        OBJ_t *ob = fr[guifr%maxframes].objs + gui_hover;
        if( ob->type != OBJT_POPUP )
        {
                POPUP_t *pop = ob->data;
                pop->hover = 0;
        }
        */

        gui_hover = gui_element_at(guifr, i_mousex, i_mousey);
}

// returns obj index of a GUI element (POPUP) or 0
int gui_element_at(Uint32 guifr, int x, int y)
{
        int i;
        x /= v_scale;
        y /= v_scale;

        for( i=0; i<maxobjs; i++ )
        {
                OBJ_t *ob = fr[guifr%maxframes].objs+i;

                if( ob->type!=OBJT_POPUP )
                        continue;

                POPUP_t *pop = ob->data;
                V *pos  = flex(ob, pos);
                V *hull = flex(ob, hull);

                if( !pop->visible || !pop->enabled )
                        continue;

                if( x<pos->x+hull[0].x || x>=pos->x+hull[1].x ||
                    y<pos->y+hull[0].y || y>=pos->y+hull[1].y )
                        continue;

                return i;
        }

        return 0;
}
