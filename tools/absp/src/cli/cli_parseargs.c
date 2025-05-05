#include <cli/cli.h>

#include <stdio.h>
#include <string.h>

void cli_errorusage(void)
{
    printf("\033[31;1merror: invalid arguments\n");

    printf("\033[0;1musage:\n\tabsp [-v] inputfile [outputfile]\n");
}

void cli_parseargs(int argc, char** argv)
{
    int i;

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

    if(i != cli_args.size-1 && i != cli_args.size-2)
        cli_errorusage();
}