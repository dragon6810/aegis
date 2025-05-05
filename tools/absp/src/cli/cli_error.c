#include <cli/cli.h>

#include <std/assert/assert.h>

void cli_error(bool fatal, const char* format, ...)
{
    va_list args;

    assert(format);

    va_start(args, format);
    cli_verror(fatal, format, args);
    va_end(args);
}