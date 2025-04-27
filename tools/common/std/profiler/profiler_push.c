#include <std/profiler/profiler.h>

#include <string.h>

#include <std/assert/assert.h>

void profiler_push(const char* name)
{
    profiler_el_t el;

    assert(strlen(name) < PROFILER_MAX_NAME);

    strcpy(el.name, name);
    clock_gettime(CLOCK_MONOTONIC, &el.start);

    LIST_PUSH(profiler_stack, el);
}