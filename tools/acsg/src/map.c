#include <map.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <parselib/parselib.h>

#include <globals.h>

tokenstate_t map_parse;

void map_nextpair(void)
{

}

void map_nextbrush(void)
{

}

bool map_parsenext(void)
{
    if(!parselib_nexttoken(&map_parse))
        return false;

    if(strcmp(map_parse.token, "{"))
    {
        fprintf(stderr, "error: expected entity entry in \"%s\".\n", sourcefilepath);
        abort();
    }

    while(1)
    {
        if(!parselib_nexttoken(&map_parse))
            return false;
        if(!strcmp(map_parse.token, "}"))
            break;
        if(!strcmp(map_parse.token, "{"))
            map_nextbrush();
        else
            map_nextpair();
    }

    return true;
}

void map_parsemap(void)
{
    FILE *ptr;
    char *buf;
    unsigned long int buflen;

    ptr = fopen(sourcefilepath, "r");
    if(!ptr)
    {
        fprintf(stderr, "error: mapfile does not exist \"%s\".\n", sourcefilepath);
        abort();
    }
    fseek(ptr, 0, SEEK_END);
    buflen = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);
    buf = malloc(buflen + 1);
    fread(buf, 1, buflen, ptr);
    buf[buflen] = 0;
    fclose(ptr);
    parselib_start(buf, &map_parse);
    free(buf);

    while(map_parsenext());

    parselib_stop(&map_parse);
}