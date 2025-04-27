#include <std/arena/arena.h>

#include <std/assert/assert.h>

void* arena_alloc(arena_t* arena, unsigned long int size)
{
    void* area;

    assert(arena);
    assert(arena->start);
    
    if(!size)
        return arena->end;

    if(arena->end - arena->start + size > arena->cap)
        return NULL;

    area = arena->end;
    arena->end += size;

    return area;
}