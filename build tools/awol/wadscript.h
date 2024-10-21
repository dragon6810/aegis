#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

#ifndef _WADSCRIPT_H
#define _WADSCRIPT_H

typedef struct
{
	char* cmd;
	char* params;
} ws_cmd_t;

typedef struct
{
    char r;
    char g;
    char b;
} rgb8_t;

typedef struct
{
    char* pixeldata;
    char name[17]; // Null terminated, max length 16
    uint32_t w, h;
    rgb8_t pallete[256];
} ws_texture_t;

extern FILE* ptr;
extern ws_cmd_t *commands;
extern ws_texture_t *textures;
extern int ncommands;
extern int ntextures;

void WS_Open();
void WS_Parse();
void WS_Run();

boolean WS_LoadBMP(char* path, char type);
boolean WS_WriteWAD();

boolean WS_NextCMD();

#endif // !_WADSCRIPT_H
