#include <parselib/parselib.h>

#include <stdlib.h>

#include <std/assert/assert.h>

void parselib_stop(tokenstate_t *tokenstate)
{
    assert(tokenstate);
    assert(tokenstate->buf);
    assert(tokenstate->pos);

    free(tokenstate->buf);
    if(tokenstate->token)
        free(tokenstate->token);
}