#include "wadscript.h"

FILE* ptr = 0;
extern ws_cmd_t commands;
extern int ncommands = 0;

void WS_Open()
{
	ptr = fopen("wadfile", "r");
	if (!ptr)
	{
		printf("wadfile not found. Aborting.\n");
		exit(1);
	}
}

void WS_Parse()
{
	if (!ptr)
		return;
}