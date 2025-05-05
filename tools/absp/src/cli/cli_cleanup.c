#include <cli/cli.h>

#include <stdlib.h>

void cli_cleanup(void)
{
    int i;

    for(i=0; i<cli_args.size; i++)
        free(cli_args.data[i]);

    LIST_FREE(cli_args);
}