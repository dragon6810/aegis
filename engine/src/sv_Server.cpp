#include <engine/sv_Server.h>

#include <stdint.h>
#include <unistd.h>

#include <utilslib.h>

void engine::sv::Server::Setup(void)
{

}

int engine::sv::Server::Run(void)
{
    uint64_t lastframe, thisframe, nextframe, now, time;

    // TODO: this is quite imprecise and slow by a few ms per tick.
    // find a better way to do this.

    lastframe = TIMEMS;
    while(1)
    {
        thisframe = TIMEMS;
        time = thisframe - lastframe;
        lastframe = thisframe;

        printf("tick time: %llu.\n", time);

        nextframe = thisframe + (uint64_t) tickms;
        now = TIMEMS;
        if(now < nextframe)
            usleep((nextframe - now) * 1000);
    }

    return 0;
}