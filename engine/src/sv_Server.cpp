#include <engine/sv_Server.h>

#include <stdint.h>
#include <unistd.h>

#include <utilslib.h>

#include <engine/Console.h>

void engine::sv::Server::Setup(void)
{

}

engine::Console::cvar_t testvar = { "sv_testvar", engine::Console::CPERM_ANYONE, "0", };

int engine::sv::Server::Run(void)
{
    uint64_t lastframe, thisframe, nextframe, now, time;

    // TODO: this is quite imprecise and slow by a few ms per tick.
    // find a better way to do this.

    Console::RegisterCVar(&testvar);

    Console::LaunchTerm();

    lastframe = TIMEMS;
    while(1)
    {
        thisframe = TIMEMS;
        time = thisframe - lastframe;
        lastframe = thisframe;

        Console::ExecTerm();

        //printf("tick time: %llu.\n", time);

        nextframe = thisframe + (uint64_t) tickms;
        now = TIMEMS;
        if(now < nextframe)
            usleep((nextframe - now) * 1000);
    }

    Console::KillTerm();

    return 0;
}