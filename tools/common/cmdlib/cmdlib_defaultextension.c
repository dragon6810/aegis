#include <cmdlib/cmdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cmdlib_defaultextension(char* path, const char* ext, int maxlen)
{
    int i;

    int pathlen, extlen;

    pathlen = strlen(path);
    extlen = strlen(ext);

    if(pathlen >= extlen)
    {
        for(i=pathlen-1; i>=pathlen-extlen; i--)
        {
            if(path[i] != ext[i-pathlen+extlen])
                break;
        }

        /* path already ends with ext */
        if(i <= pathlen-extlen)
            return;
    }

    if(pathlen + extlen >= maxlen)
    {
        fprintf(stderr, "error: default extension exceeds maximum length: \"%s%s\"\n", path, ext);
        abort();
    }

    strcat(path, ext);
}