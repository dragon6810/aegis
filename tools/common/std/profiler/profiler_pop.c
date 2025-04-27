#include <std/profiler/profiler.h>

#include <std/assert/assert.h>

void profiler_pop(void)
{
    profiler_el_t el;
    struct timespec now;
    float elapsedms;

    assert(profiler_stack.size);

    LIST_POP(profiler_stack, &el);
    clock_gettime(CLOCK_MONOTONIC, &now);

    elapsedms = 0.0;
    elapsedms += (float) (now.tv_sec - el.start.tv_sec) * 1000.0;
    elapsedms += (float) (now.tv_nsec - el.start.tv_nsec) / 1000000.0;

    printf("PROFILER: Task \"%s\" completed in %fms\n", el.name, elapsedms);
}