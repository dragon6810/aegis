#include <std/timer/timer.h>

void timer_begin(void)
{
    clock_gettime(CLOCK_MONOTONIC, &timer_starttime);
}
