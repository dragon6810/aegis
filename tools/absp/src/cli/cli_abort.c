#include <cli/cli.h>

#include <stdio.h>
#include <stdlib.h>

void cli_abort(void)
{
    printf("\n\033[0;1maborting with %d errors\n\n", cli_nerrors);
    
    printf("\033[0m");

    abort();
}