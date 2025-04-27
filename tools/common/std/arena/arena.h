#ifndef _arena_h
#define _arena_h

typedef struct arena_s arena_t;

struct arena_s
{
    void *start, *end;
    unsigned long int cap;
};

void arena_initialize(arena_t* arena, unsigned long int size);
void* arena_alloc(arena_t* arena, unsigned long int size);
void arena_reset(arena_t* arena);
void arena_free(arena_t* arena);

#endif