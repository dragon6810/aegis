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
    for(i=0; i<bm->h; i++)
    {
        fread(bm->data + i * bm->w, 1, bm->w, ptr);
        fseek(ptr, (4 - (bm->w % 4)) % 4, SEEK_CUR);
    }

    fclose(ptr);
    return bm;
}

boolean WriteBitmap(char* path, bitmap_t* bm)
{
    int i, x, y;

    FILE* ptr;
    
    char signature[2] = "BM";
    int filesize, datasize;
    int misc;
    int64_t zero = 0;

    if(!bm)
    {
        printf("Can not write bitmap \"%s\": null bitmap given.\n", path);
        return false;
    }

    ptr = fopen(path, "wb");
    if(!ptr)
    {
        printf("Can not write bitmap \"%s\": can't open file for writing.\n", path);
        return false;
    }

    datasize = (bm->w + ((4 - (bm->w % 4)) % 4)) * bm->h;
    filesize = 54 + 256 * 4 + datasize;

    fwrite(signature, 1, 2, ptr);
    fwrite(&filesize, sizeof(int), 1, ptr);
    fwrite(&zero, 4, 1, ptr);
    filesize -= datasize;
    fwrite(&filesize, sizeof(int), 1, ptr);
    filesize += datasize;

    misc = 40;
    fwrite(&misc, sizeof(int), 1, ptr);
    fwrite(&bm->w, sizeof(int), 1, ptr);
    fwrite(&bm->h, sizeof(int), 1, ptr);
    misc = 1;
    fwrite(&misc, 2, 1, ptr);
    misc = 8;
    fwrite(&misc, 2, 1, ptr);
    fwrite(&zero, 4, 1, ptr);
    fwrite(&datasize, sizeof(int), 1, ptr);
    misc = 1;
    fwrite(&misc, 4, 1, ptr);
    fwrite(&misc, 4, 1, ptr);
    misc = 256;
    fwrite(&misc, 4, 1, ptr);
    fwrite(&zero, 4, 1, ptr);

    for(i=0; i<256; i++)
    {
        fwrite(&bm->palette[i].r, 1, 1, ptr);
        fwrite(&bm->palette[i].g, 1, 1, ptr);
        fwrite(&bm->palette[i].b, 1, 1, ptr);
        fwrite(&zero, 1, 1, ptr);
    }

    for(y=bm->h-1; y>=0; y--)
    {
        for(x=0; x<bm->w; x++)
        {
            printf("%d ", bm->data[y * bm->w + x]);
            fwrite(&bm->data[y * bm->w + x], 1, 1, ptr);
        }
        fwrite(&zero, (4 - (bm->w % 4)) % 4, 1, ptr);
        printf("\n");  
    }

    fclose(ptr);
}