#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmdlib/cmdlib.h>
#include <std/profiler/profiler.h>

#include <csg.h>
#include <globals.h>
#include <map.h>
#include <entity.h>

int main(int argc, char** argv)
{
    int i;

    profiler_setup();

    for(i=1; i<argc; i++)
    {
        if(argv[i][0] != '-')
            break;
    }

    if(i != argc - 2 && i != argc - 1)
    {
        fprintf(stderr, "usage: acsg [options] sourcefile [destfile]\n");
        fprintf(stderr, "options: -nojunc -nofill -threads[124] -draw -onlyents -verbose -proj <projectpath>\n");
        abort();
    }

    if(strlen(argv[i]) >= sizeof(sourcefilepath))
    {
        fprintf(stderr, "invalid file path \"%s\"\n", argv[i]);
        abort();
    }
    strcpy(sourcefilepath, argv[i]);
    cmdlib_defaultextension(sourcefilepath, ".map", sizeof(sourcefilepath));

    if(i != argc - 2)
    {
        strcpy(destfilepath, sourcefilepath);
        cmdlib_stripextension(destfilepath);
        cmdlib_defaultextension(destfilepath, ".bsp", sizeof(destfilepath));
    }
    else
    {
        if(strlen(argv[i+1]) >= sizeof(destfilepath))
        {
            fprintf(stderr, "invalid file path \"%s\"\n", argv[i+1]);
            abort();
        }
        strcpy(destfilepath, argv[i+1]);
        cmdlib_defaultextension(destfilepath, ".bsp", sizeof(destfilepath));
    }

    strcpy(entfilepath, sourcefilepath);
    cmdlib_stripextension(entfilepath);
    cmdlib_defaultextension(entfilepath, ".ent", sizeof(entfilepath));

    map_parsemap();
    csg_docsg();
    entity_writeents();
}