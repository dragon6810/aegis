#include <stdio.h>
#include "defs.h"

#ifndef _WADSCRIPT_H
#define _WADSCRIPT_H

typedef struct
{
	char* cmd;
	char* params;
} ws_cmd_t;

extern FILE* ptr;
extern ws_cmd_t commands;
extern int ncommands;

void WS_Open();
void WS_Parse();

boolean WS_NextCMD();

#endif // !_WADSCRIPT_H
