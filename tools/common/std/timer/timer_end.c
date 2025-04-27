#include <std/timer/timer.h>

float timer_end(void)
{
    clock_gettime(CLOCK_MONOTONIC, &timer_endtime);

    timer_elapsedms = 0.0;
    timer_elapsedms += (float) (timer_endtime.tv_sec - timer_starttime.tv_sec) * 1000.0;
    timer_elapsedms += (float) (timer_endtime.tv_nsec - timer_starttime.tv_nsec) / 1000000.0;

    return timer_elapsedms;
}
