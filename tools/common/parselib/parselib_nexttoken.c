#include <parselib/parselib.h>

#include <string.h>

#include <std/assert/assert.h>

bool parselib_nexttoken(tokenstate_t *tokenstate)
{
    int len;
    char *start;

    assert(tokenstate);
    assert(tokenstate->buf);
    assert(tokenstate->pos);

    if(tokenstate->token)
        free(tokenstate->token);
    tokenstate->token = NULL;

    while(1)
    {
        while(*tokenstate->pos <= 32)
        {
            if(!*tokenstate->pos)
                return false;

            tokenstate->pos++;
        }

        if(*tokenstate->pos != '#')
            break;

        while(*tokenstate->pos != '\n')
        {
            if(!*tokenstate->pos)
                return false;

            tokenstate->pos++;
        }
    }

    start = tokenstate->pos;
    if(*tokenstate->pos == '"')
    {
        do
            tokenstate->pos++;
        while(*tokenstate->pos != '"' && *tokenstate->pos != '\n' && *tokenstate->pos);
        if(*tokenstate->pos == '"')
            tokenstate->pos++;
    }
    else
    {
        while(*tokenstate->pos > 32)
            tokenstate->pos++;
    }
    len = tokenstate->pos - start;
    tokenstate->token = malloc(len + 1);
    memcpy(tokenstate->token, start, len);
    tokenstate->token[len] = 0;

    return true;
}