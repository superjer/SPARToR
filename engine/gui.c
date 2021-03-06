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

void gui_update(unsigned int guifr)
{
        /*
        object *ob = fr[guifr%maxframes].objs + gui_hover;
        if( ob->type != popup_type )
        {
                popup *pop = ob->data;
                pop->hover = 0;
        }
        */

        gui_hover = gui_element_at(guifr, i_mousex, i_mousey);
}

// returns obj index of a GUI element (popup) or 0
int gui_element_at(unsigned int guifr, int x, int y)
{
        int i;
        x /= v_scale;
        y /= v_scale;

        for( i=0; i<maxobjs; i++ )
        {
                object *ob = fr[guifr%maxframes].objs+i;

                if( ob->type != popup_type )
                        continue;

                popup *pop = ob->data;
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
