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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dlfcn.h>
#include "mod.h"
#include "audio.h"

draw_object_sig(slug)
{
        slug *sl = o->data;

        if( sl->vel.x>0 )
                sprblit( sl->dead ? &SM(slug_ouch_r) : &SM(slug_r), sl->pos.x, sl->pos.y );
        else
                sprblit( sl->dead ? &SM(slug_ouch_l) : &SM(slug_l), sl->pos.x, sl->pos.y );
}

advance_object_sig(slug)
{
        typedef void (*ftype)(FRAME_t *, int, int, unsigned int, unsigned int, object *, object *);
        static ftype fp;
        static time_t mtime;
        static void *handle;
        static char so_file[] = "./mcdiddy.so";

        // FIXME! clean up with: dlclose(handle);

        struct stat fileinfo;
        if (stat(so_file, &fileinfo))
        {
                echo("Failed to stat %s", so_file);
                return;
        }

        if (fileinfo.st_mtime != mtime)
        {
                echo("New mtime is %d", fileinfo.st_mtime);
                if (handle)
                {
                        echo("Closing so file %s", so_file);
                        if (dlclose(handle))
                                echo("Error closing so file %s", so_file);
                }
                echo("Opening so file %s", so_file);
                handle = dlopen(so_file, RTLD_NOW);
                if (!handle)
                        echo("dlopen error: %s", dlerror());
        }

        mtime = fileinfo.st_mtime;

        if (!handle)
                return;

        fp = (ftype)dlsym(handle, "real_function");

        char *error;
        if ((error = dlerror()))
        {
                echo(error);
                return;
        }

        fp(fr, maxobjs, objid, a, b, oa, ob);
}
