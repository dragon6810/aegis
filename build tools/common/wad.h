#include "bitmap.h"

typedef struct
{
	char type;
	char name[16];
	bitmap_t* bm;
} image_t;

typedef struct
{
    char name[16];
    bitmap_t *glyfs[256];
	rgb8_t palette[256];
	int w, h;
} font_t;

void WriteWad(char* path, image_t* images, int nimages, font_t* fonts, int nfonts);