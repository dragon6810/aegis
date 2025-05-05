#include <stdio.h>

#include <cli/cli.h>

int main(int argc, char** argv)
{
    cli_welcomemessage();

    cli_parseargs(argc, argv);

    cli_cleanup();

    return 0;
}