#include <std/profiler/profiler.h>

void profiler_setup(void)
{
    LIST_INITIALIZE(profiler_stack);
}