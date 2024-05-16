#pragma once

typedef enum
{
	TEXTYPE_MIPTEX,
	TEXTYPE_MSTUDIO
} texturetype_t;


void loadmstudiotexture(char* data, int which, texturetype_t type, int** out, int* width, int* height);