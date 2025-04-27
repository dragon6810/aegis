#include <std/textutils/textutils.h>

#include <stdlib.h>
#include <string.h>

char* textutils_insert(char* str, const char* insert, int pos)
{
    char *newstr;
    unsigned long int newlen;
    unsigned long int insertlen;

    if(!str)
        return NULL;
    if(!insert)
        return NULL;

    newlen = strlen(str) + strlen(insert);
    newstr = malloc(newlen + 1);

    insertlen = strlen(insert);

    memcpy(newstr, str, pos);
    memcpy(newstr + pos, insert, insertlen);
    if(pos < strlen(str))
        memcpy(newstr + pos + insertlen, str + pos, strlen(str) - pos);
    newstr[newlen] = 0;

    free(str);

    return newstr;
}