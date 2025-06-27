#include <cli/cli.h>

int cli_nerrors = 0;

list_str_t cli_args = {};
bool cli_verbose = false;
bool cli_keepinput = false;
char cli_infiles[MAX_MAP_HULLS][PATH_MAX] = { "", "", "", "", };
char cli_entfile[PATH_MAX] = "";
char cli_outfile[PATH_MAX] = "";