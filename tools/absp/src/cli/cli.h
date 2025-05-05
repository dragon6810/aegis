#ifndef _CLI_H
#define _CLI_H

#include <limits.h>
#include <stdbool.h>

#include <bspfile.h>
#include <std/list/list.h>
#include <std/list/list_types.h>

extern list_str_t cli_args;
extern bool cli_verbose;

extern char cli_infiles[MAX_MAP_HULLS][PATH_MAX];

void cli_welcomemessage(void);
void cli_parseargs(int argc, char** argv);
void cli_cleanup(void);

#endif