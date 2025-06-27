#include <stdio.h>

#include <std/profiler/profiler.h>

#include <bsp/bsp.h>
#include <cli/cli.h>

int main(int argc, char** argv)
{
    int i;

    cli_welcomemessage();

    profiler_setup();

    cli_parseargs(argc, argv);
    
    bsp_loadhulls();
    bsp_loadents();
    
    bsp_partition();
    bsp_loadintofile();
    bspfile_writebspfile(cli_outfile);

    if(!cli_keepinput)
    {
        for(i=0; i<MAX_MAP_HULLS; i++)
            remove(cli_infiles[i]);
        remove(cli_entfile);       
    }

    cli_cleanup();

    return 0;
}