#include <parselib/parselib.h>

#include <string.h>

#include <std/assert/assert.h>

void parselib_start(char *buf, tokenstate_t *tokenstate)
{
    assert(buf);
    assert(tokenstate);

    memset(tokenstate, 0, sizeof(tokenstate_t));
    tokenstate->buf = tokenstate->pos = strdup(buf);
}