#include <stdio.h>
#include <string.h>

#include "textfile.h"
#include "wad.h"

#define MAXIMG 1024

char cdtexture[64];
char outwad[64];
image_t images[MAXIMG];
int nimages = 0;

void ProcessTexture(char* path, int type)
{
	FILE* ptr;
	char* realpath;
	image_t img;

	if(strlen(path) > 15)
	{
		printf("Texture name too long, max is 15 characters \"%s\".\n", path);
		return;
	}

	realpath = calloc(strlen(path) + 1 + strlen(cdtexture) + 4 + 1, 1);
	strcpy(realpath, cdtexture);
	if (cdtexture[strlen(cdtexture) - 1] != '/' && cdtexture[strlen(cdtexture) - 1] != '\\')
		strcat(realpath, "/");
	strcat(realpath, path);
	strcat(realpath, ".bmp");

	img.bm = LoadBitmap(realpath);
	img.type = type;
	strcpy(img.name, path);
	
	if(nimages >= MAXIMG)
	{
		printf("Too many images! Max is %d.\n", MAXIMG);
		free(realpath);
		return;
	}

	images[nimages++] = img;

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

		if (!strcmp(token, "$wad"))
		{
			NextToken();
			strcpy(outwad, token);
			continue;
		}
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
		if (!strcmp(token, "$pic"))
		{
			NextToken();
			ProcessTexture(token, 0x42);
			continue;
		}
	}

	if(!strcmp(outwad, ""))
		printf("Now wad output specified!\n");
	else
		WriteWad(outwad, images, nimages);
}