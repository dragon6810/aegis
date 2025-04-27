#include <cmdlib/cmdlib.h>

#include <string.h>

#include <std/assert/assert.h>

void cmdlib_stripextension(char* path)
{
    int i;
    unsigned pathlen;

    assert(path);
    
    pathlen = strlen(path);
    for(i=pathlen-1; i>=0; i--)
        if(path[i] == '.')
            break;

    if(path[i] != '.')
        return;

    memset(path + i, 0, pathlen - i);
}