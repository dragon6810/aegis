#include "loadtexture.h"

#include <iostream>

#include "defs.h"
#include "binaryloader.h"

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
	mstudioheader_t* pheader = (mstudioheader_t*)(data);
	
	int ptexoffset = ((mstudioheader_t*)(data))->textureindex;
	mstudiotexture_t* ptex = (mstudiotexture_t*)(data + ptexoffset) + which;
	*width = ptex->width;
	*height = ptex->height;
	printf("Loading texture \"%s\".\n", ptex->name);
	palette_t* pallete = (palette_t*)(data + ptex->index + ptex->width * ptex->height);
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
		(*out)[p] = moderncolors[*((unsigned char*)(data + ptex->index + p))];

	return;
}

void loadmiptex(char* data, int** out, int* width, int* height)
{
	miptex_t* texture = (miptex_t*)data;

	*width = texture->width;
	*height = texture->height;
	printf("Loading texture \"%s\".\n", texture->name);
	int palleteIndex = texture->offsets[BSP_MIPLEVELS - 1] + (*width >> (BSP_MIPLEVELS - 1)) * (*height >> (BSP_MIPLEVELS - 1));

	miptexpalette_t* pallete = (miptexpalette_t*)((char*)texture + palleteIndex);
	int moderncolors[COLORSINPALETTE]{};
	for (int c = 0; c < COLORSINPALETTE; c++)
	{
		int col = 0;
		col |= ((int)pallete->colors[c].r) << 0;
		col |= ((int)pallete->colors[c].g) << 8;
		col |= ((int)pallete->colors[c].b) << 16;

		if(!(pallete->colors[c].r == 0 && pallete->colors[c].g == 0 && pallete->colors[c].b == 255))
			col |= 0xFF000000;

		moderncolors[c] = col;
	}

	for (int i = 0; i < BSP_MIPLEVELS; i++)
	{
		out[i] = (int*)malloc((*width >> i) * (*height >> i) * sizeof(int));

		for (int p = 0; p < ((*width >> i) * (*height >> i)); p++)
			out[i][p] = moderncolors[*((unsigned char*)texture + texture->offsets[i] + p)];
	}
}