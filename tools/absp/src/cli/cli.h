#ifndef _CLI_H
#define _CLI_H

#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>

#include <bspfile.h>
#include <std/list/list.h>
#include <std/list/list_types.h>

extern int cli_nerrors;

extern list_str_t cli_args;
extern bool cli_verbose;
extern bool cli_keepinput;
extern char cli_infiles[MAX_MAP_HULLS][PATH_MAX];
extern char cli_entfile[PATH_MAX];
extern char cli_outfile[PATH_MAX];

void cli_error(bool fatal, const char* format, ...);
void cli_verror(bool fatal, const char* format, va_list args);
void cli_abort(void);
void cli_welcomemessage(void);
void cli_parseargs(int argc, char** argv);
void cli_cleanup(void);

#endif