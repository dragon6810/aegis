#include <std/textutils/textutils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* textutils_remove(char* str, int start, int end)
{
    char *newstr;
    int len;

    len = strlen(str);
    newstr = malloc(len - (end - start) + 1);
    memcpy(newstr, str, start);
    memcpy(newstr + start, str + end, len - end);
    newstr[len - (end - start)] = 0;

    free(str);

    return newstr;
}