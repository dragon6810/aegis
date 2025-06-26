#include <cli/cli.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cmdlib/cmdlib.h>

void cli_errorusage(void)
{
    printf("\033[31;1merror: invalid arguments\n");

    printf("\033[0;1musage:\n\tabsp [-v] inputfile [outputfile]\n");
    cli_nerrors++;

    printf("\033[0m");
}

void cli_errornofile(const char* filename)
{
    printf("\033[31;1merror: no file \"%s\"\n", filename);
    cli_nerrors++;

    printf("\033[0m");
}

void cli_parseargs(int argc, char** argv)
{
    int i, h;

    char ext[4];

    LIST_INITIALIZE(cli_args);
    LIST_RESIZE(cli_args, argc);

    for(i=0; i<argc; i++)
        cli_args.data[i] = strdup(argv[i]);

    for(i=1; i<cli_args.size; i++)
    {
        if(cli_args.data[i][0] != '-')
            break;

        if
        (
            !strcmp(cli_args.data[i], "-v") ||
            !strcmp(cli_args.data[i], "-V") ||
            !strcmp(cli_args.data[i], "-verbose")
        )
        {
            printf("running in verbose mode\n");
            cli_verbose = true;
        }
    }

    printf("\n");

    if(i != cli_args.size-1 && i != cli_args.size-2)
        cli_errorusage();
    
    if(cli_nerrors)
        cli_abort();

    if(strlen(cli_args.data[i]) >= PATH_MAX)
    {
        printf("\033[31;1merror: input file name too long\n");
        abort();
    }

    strcpy(ext, ".g");
    ext[3] = 0;

    if(cli_verbose)
        printf("input files:\n");

    for(h=0; h<MAX_MAP_HULLS; h++)
    {
        strcpy(cli_infiles[h], cli_args.data[i]);
        cmdlib_stripextension(cli_infiles[h]);
        ext[2] = '0' + h;
        cmdlib_defaultextension(cli_infiles[h], ext, sizeof(cli_infiles[h]));
        if(cli_verbose)
            printf("    %s\n", cli_infiles[h]);
    }

    strcpy(cli_entfile, cli_args.data[i]);
    cmdlib_stripextension(cli_entfile);
    cmdlib_defaultextension(cli_entfile, ".ent", sizeof(cli_entfile));
    if(cli_verbose)
            printf("    %s\n", cli_entfile);

    if(cli_verbose)
        printf("\n");

    for(h=0; h<MAX_MAP_HULLS; h++)
    {
        if(!access(cli_infiles[h], F_OK))
            continue;

        cli_errornofile(cli_infiles[h]);
    }

    if(access(cli_entfile, F_OK))
        cli_errornofile(cli_entfile);

    if(cli_verbose)
        printf("output files:\n");

    strcpy(cli_outfile, cli_args.data[i]);
    cmdlib_stripextension(cli_outfile);
    cmdlib_defaultextension(cli_outfile, ".bsp", sizeof(cli_outfile));
    if(cli_verbose)
            printf("    %s\n", cli_outfile);

    if(cli_verbose)
        printf("\n");

    if(cli_nerrors)
        cli_abort();
}