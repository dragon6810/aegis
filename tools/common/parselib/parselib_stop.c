#include <parselib/parselib.h>

#include <stdlib.h>
#include <string.h>

#include <std/assert/assert.h>

void parselib_stop(tokenstate_t *tokenstate)
{
    assert(tokenstate);
    assert(tokenstate->buf);
    assert(tokenstate->pos);

    free(tokenstate->buf);
    if(tokenstate->token)
        free(tokenstate->token);
    memset(tokenstate, 0, sizeof(tokenstate_t));
}