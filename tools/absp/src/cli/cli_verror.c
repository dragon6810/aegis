#include <cli/cli.h>

#include <std/assert/assert.h>

void cli_verror(bool fatal, const char* format, va_list args)
{
    assert(format);

    printf("\033[31;1m");
    if(fatal)
        printf("fatal ");
    printf("error: ");
    vprintf(format, args);

    cli_nerrors++;
    if(fatal)
        cli_abort();
}