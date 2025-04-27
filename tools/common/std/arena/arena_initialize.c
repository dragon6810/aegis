#include <std/arena/arena.h>

#include <std/assert/assert.h>

void arena_initialize(arena_t* arena, unsigned long int size)
{
    assert(arena);
    
    arena->start = malloc(size);
    arena->end = arena->start;
    arena->cap = size;
}