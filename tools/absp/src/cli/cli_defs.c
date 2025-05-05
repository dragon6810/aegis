#include <cli/cli.h>

list_str_t cli_args = {};
bool cli_verbose = false;
char cli_infiles[MAX_MAP_HULLS][PATH_MAX] = { "", "", "", "", };
char cli_entfile[PATH_MAX] = "";