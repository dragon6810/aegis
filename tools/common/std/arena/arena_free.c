#include <std/arena/arena.h>

#include <std/assert/assert.h>

void arena_free(arena_t* arena)
{
    assert(arena);
    assert(arena->start);

    free(arena->start);
}