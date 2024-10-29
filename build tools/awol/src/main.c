#include <stdio.h>
#include <string.h>

#include "textfile.h"
#include "bitmap.h"

char cdtexture[64];

void ProcessTexture(char* path, int type)
{
	FILE* ptr;
	char* realpath;

	realpath = calloc(strlen(path) + 1 + strlen(cdtexture) + 4 + 1, 1);
	strcpy(realpath, cdtexture);
	if (cdtexture[strlen(cdtexture) - 1] != '/' && cdtexture[strlen(cdtexture) - 1] != '\\')
		strcat(realpath, "/");
	strcat(realpath, path);
	strcat(realpath, ".bmp");

	LoadBitmap(realpath);

	free(realpath);
}

int main(int argc, char** argv)
{
	LoadScript(argv[argc - 1]);

	while (NextToken())
	{
		if (token[0] != '$')
			continue;

		if (!TokenAvailable())
			continue;

		if (!strcmp(token, "$cdtexture"))
		{
			NextToken();
			strcpy(cdtexture, token);
			continue;
		}
		if (!strcmp(token, "$texture"))
		{
			NextToken();
			ProcessTexture(token, 0x43);
			continue;
		}
	}
}