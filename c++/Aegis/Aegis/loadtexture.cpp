#include "loadtexture.h"

#include <iostream>

#include "defs.h"

void loadmstudiotexture(char* data, int texoffset, texturetype_t type, int** out, int* width, int* height)
{
	switch (type)
	{
	case TEXTYPE_MSTUDIO:
		goto mdltex;
	default:
		return;
	}

mdltex:
	mstudiotexture_t* ptex = (mstudiotexture_t*)(data + texoffset);
	*width = ptex->width;
	*height = ptex->height;
	printf("Loading texture \"%s\".\n", ptex->name);
	palette_t* pallete = (palette_t*)(data + texoffset + ptex->width * ptex->height);
	*out = (int*) malloc(ptex->width * ptex->height * sizeof(int));
	int moderncolors[COLORSINPALETTE]{};
	for (int c = 0; c < COLORSINPALETTE; c++)
	{
		int col = 0;
		col |= ((int)pallete->colors[c].r) << 24;
		col |= ((int)pallete->colors[c].g) << 16;
		col |= ((int)pallete->colors[c].b) <<  8;
		col |= 0x000000FF;

		moderncolors[c] = col;
	}

	for (int p = 0; p < ptex->width * ptex->height; p++)
		(*out)[p] = moderncolors[*(unsigned char*)(data + ptex->index + p)];

	return;
}