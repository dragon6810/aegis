#include <stdio.h>
#include <string.h>

#include "textfile.h"
#include "wad.h"

#ifdef WIN32
#include <io.h>
#define F_OK 0
#define access _access
#else
#include <unistd.h>
#endif

#define MAXIMG 1024
#define MAXFONT 64

char cdtexture[64];
char outwad[64];
image_t images[MAXIMG];
font_t fonts[MAXFONT];
int nimages = 0;
int nfonts = 0;

void ProcessFont(char* path)
{
	int i;

	char *realpath;
	char *curfilepath;
	font_t *font;

	if(nfonts + 1 > MAXFONT)
	{
		printf("Too many fonts, max is %d.\n", MAXFONT);
		return;
	}

	if(strlen(path) > 15)
	{
		printf("Font name \"%s\" is too long, max is 15 characters.\n", path);
		return;
	}

	if(path[strlen(path)-1] != '/' && path[strlen(path)-1] != '\\')
	{
		realpath = malloc(strlen(path + 2));
		strcpy(realpath, path);
		strcat(realpath, "/");
	}
	else
		realpath = path;

	printf("Processing font at \"%s\".\n", realpath);
	font = &fonts[nfonts++];
	strcpy(font->name, path);

	curfilepath = malloc(strlen(realpath) + 3 + 4 + 1);
	for(i=0, font->w=0; i<256; i++)
	{
		memset(curfilepath, 0, strlen(curfilepath));
		sprintf(curfilepath, "%s%d.bmp", realpath, i);
		if(access(curfilepath, F_OK))
			continue;
		
		font->glyfs[i] = LoadBitmap(curfilepath);
		font->w += font->glyfs[i]->w;
		font->h = font->glyfs[i]->h;
		memcpy(font->palette, font->glyfs[i]->palette, 256 * 3);
	}

	free(curfilepath);

	if(realpath != path)
		free(realpath);
}

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
		if (!strcmp(token, "$font"))
		{
			NextToken();
			ProcessFont(token);
			continue;
		}
	}

	if(!strcmp(outwad, ""))
		printf("Now wad output specified!\n");
	else
		WriteWad(outwad, images, nimages, fonts, nfonts);
}