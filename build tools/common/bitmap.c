#include "bitmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bitmap_t* LoadBitmap(char* path)
{
    int i;

    bitmap_t *bm;
    FILE* ptr;

    char signature[3];
    short bitdepth;
    int compression;
    int ncolors;

    ptr = fopen(path, "rb");
    if(!ptr)
    {
        printf("Can't find file \"%s\".\n", path);
        return NULL;
    }

    memset(signature, 0, sizeof(signature));
    fread(signature, 1, 2, ptr);
    if(strcmp(signature, "BM"))
    {
        printf("Invalid bitmap file \"%s\".\n", path);
        fclose(ptr);
        return NULL;
    }

    bm = malloc(sizeof(bitmap_t));
    memset(bm, 0, sizeof(bitmap_t));

    fseek(ptr, 16, SEEK_CUR);
    fread(&bm->w, sizeof(int), 1, ptr);
    fread(&bm->h, sizeof(int), 1, ptr);

    printf("Bitmap \"%s\" has dimensions %dx%d.\n", path, bm->w, bm->h);

    fseek(ptr, 2, SEEK_CUR);
    fread(&bitdepth, sizeof(short), 1, ptr);
    fread(&compression, sizeof(int), 1, ptr);

    if(bitdepth != 8 || compression)
    {
        printf("Invalid bitmap \"%s\", all bitmaps must be 8-bit uncompressed.\n", path);
        free(bm);
        fclose(ptr);
        return NULL;
    }

    fseek(ptr, 12, SEEK_CUR);
    fread(&ncolors, sizeof(int), 1, ptr);
    fseek(ptr, 4, SEEK_CUR);

    for(i=0; i<ncolors; i++)
    {
        fread(&bm->palette[i].b, 1, 1, ptr);
        fread(&bm->palette[i].g, 1, 1, ptr);
        fread(&bm->palette[i].r, 1, 1, ptr);
        fseek(ptr, 1, SEEK_CUR);
    }

    bm->data = malloc(bm->w * bm->h);
    for(i=0; i<bm->w*bm->h; i++)
        fread(bm->data + i, 1, 1, ptr);

    fclose(ptr);
    return bm;
}