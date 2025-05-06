#include <stdio.h>

#include <std/profiler/profiler.h>

#include <bsp/bsp.h>
#include <cli/cli.h>

int main(int argc, char** argv)
{
    cli_welcomemessage();

    profiler_setup();

    cli_parseargs(argc, argv);
    
    bsp_loadhulls();
    bsp_loadents();

    cli_cleanup();

    return 0;
}