#include <cmdlib/cmdlib.h>

#include <string.h>

#include <std/assert/assert.h>

void cmdlib_stripextension(char* path)
{
    int i;
    unsigned pathlen;

    assert(path);
    
    pathlen = strlen(path);
    if(!pathlen)
        return;
    
    for(i=pathlen-1; i>=0; i--)
        if(path[i] == '.')
            break;
    
    if(i < 0 || path[i] != '.')
        return;

    memset(path + i, 0, pathlen - i);
}