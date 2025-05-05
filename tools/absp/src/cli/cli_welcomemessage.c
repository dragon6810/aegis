#include <cli/cli.h>

#include <stdio.h>

void cli_welcomemessage(void)
{
    const int npad = 32;

    int i;

    char pad[npad+1];

    for(i=0; i<npad; i++)
        pad[i] = '-';
    pad[i] = 0;

    printf("\n%s ABSP %s\n\n", pad, pad);
}