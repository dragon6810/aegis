#include <std/arena/arena.h>

#include <std/assert/assert.h>

void arena_reset(arena_t* arena)
{
    assert(arena);
    assert(arena->start);

    arena->end = arena->start;
}