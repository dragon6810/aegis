#include "loadtexture.h"

#include <iostream>

#include "defs.h"

void loadmstudiotexture(char* data, int which, texturetype_t type, int** out, int* width, int* height)
{
	switch (type)
	{
	case TEXTYPE_MSTUDIO:
		goto mdltex;
	default:
		return;
	}

mdltex:
	/*
	mstudioheader_t* pheader = (mstudioheader_t*)(data);
	if (pheader->numtextures == 0)
	{
		char texturename[256];

		strcpy(texturename, pheader->name);
		strcpy(&texturename[strlen(texturename) - 4], "T.mdl");

		data = LoadModel(texturename);
	}
	*/
	int ptexoffset = ((mstudioheader_t*)(data))->textureindex;
	mstudiotexture_t* ptex = (mstudiotexture_t*)(data + ptexoffset) + which;
	*width = ptex->width;
	*height = ptex->height;
	printf("Loading texture \"%s\".\n", ptex->name);
	palette_t* pallete = (palette_t*)(data + ptexoffset + ptex->width * ptex->height);
	*out = (int*) malloc(ptex->width * ptex->height * sizeof(int));
	int moderncolors[COLORSINPALETTE]{};
	for (int c = 0; c < COLORSINPALETTE; c++)
	{
		int col = 0;
		col |= ((int)pallete->colors[c].r) <<  0;
		col |= ((int)pallete->colors[c].g) <<  8;
		col |= ((int)pallete->colors[c].b) << 16;
		col |= 0xFF000000;

		moderncolors[c] = col;
	}

	for (int p = 0; p < ptex->width * ptex->height; p++)
		(*out)[p] = moderncolors[*(unsigned char*)(data + ptex->index + p)];

	return;
}