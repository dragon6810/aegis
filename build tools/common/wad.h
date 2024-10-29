#include "bitmap.h"

typedef struct
{
	char type;
	char name[16];
	bitmap_t* bm;
} image_t;

void WriteWad(char* path, image_t* images, int nimages);