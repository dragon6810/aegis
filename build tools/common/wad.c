#include "wad.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void WriteWad(char* path, image_t* images, int nimages, font_t* fonts, int nfonts)
{
    int i, j, k, l, m;
    image_t *curimg;
    font_t *curfont;

    FILE* ptr;
    
    char magic[4] = "WAD3";
    int datasize;
    int *dataoffsets;
    int *datasizes;
    short ncolors = 256;
    int offs;
    int misc;

    ptr = fopen(path, "wb");
    if(!ptr)
    {
        printf("Failed to open wad output \"%s\".\n", path);
        return;
    }

    // Make sure names are null-padded
    for(i=0, curimg=images; i<nimages; i++, curimg++)
    {
        l = strlen(curimg->name);
        if(l >= 15)
            continue;

        for(j=l; j<16; j++)
            curimg->name[j] = 0;
    }

    dataoffsets = malloc(sizeof(int) * nimages);
    datasizes = malloc(sizeof(int) * nimages);
    for(i=0, datasize=0, curimg=images; i<nimages; i++, curimg++)
    {
        dataoffsets[i] = datasize + 12;
        switch(curimg->type)
        {
            case 0x42:
                datasizes[i] += 12;
                datasizes[i] += 256 * 3;
                datasizes[i] += curimg->bm->w * curimg->bm->h;

                datasize += datasizes[i];
                break;
            case 0x43:
                datasizes[i] += 44;
                datasizes[i] += 256 * 3;
                for(j=0; j<4; j++)
                    datasizes[i] += (curimg->bm->w>>j) * (curimg->bm->h>>j);

                datasize += datasizes[i];
                break;
            default:
                printf("Unrecognized texture type %d.\n", curimg->type);
                break;
        }
    }

    for(i=nimages, curfont=fonts; i<nfonts+nimages; i++, curfont++)
    {
        dataoffsets[i] = datasize + 12;

        datasizes[i] = 16 + 4 * 256 + 2 + 3 * 256;
        datasizes[i] += curfont->w * curfont->h;

        datasize += datasizes[i];
    }

    fwrite(magic, 1, 4, ptr);
    misc = nimages + nfonts;
    fwrite(&misc, sizeof(int), 1, ptr);
    datasize += 12;
    fwrite(&datasize, sizeof(int), 1, ptr);
    datasize -= 12;

    for(i=0, curimg=images; i<nimages; i++, curimg++)
    {
        switch(curimg->type)
        {
            case 0x42:
                fwrite(&curimg->bm->w, sizeof(int), 1, ptr);
                fwrite(&curimg->bm->h, sizeof(int), 1, ptr);
                for(k=curimg->bm->h-1; k>=0; k--)
                {
                    for(j=0; j<curimg->bm->w; j++)
                    {
                        l = k * curimg->bm->w + j;
                        fwrite(curimg->bm->data + l, 1, 1, ptr);
                    }
                }
                fwrite(&ncolors, sizeof(short), 1, ptr);
                for(j=0; j<256; j++)
                {
                    fwrite(&curimg->bm->palette[j].b, 1, 1, ptr);
                    fwrite(&curimg->bm->palette[j].g, 1, 1, ptr);
                    fwrite(&curimg->bm->palette[j].r, 1, 1, ptr);
                }
                fwrite(&ncolors, sizeof(short), 1, ptr);

                break;
            case 0x43:
                fwrite(curimg->name, 1, 16, ptr);
                fwrite(&curimg->bm->w, sizeof(int), 1, ptr);
                fwrite(&curimg->bm->h, sizeof(int), 1, ptr);
                for(m=0, offs=40; m<4; m++)
                {
                    fwrite(&offs, sizeof(int), 1, ptr);
                    offs += (curimg->bm->w >> m) * (curimg->bm->h >> m);
                }

                for(m=0; m<4; m++)
                {
                    for(k=curimg->bm->h-1; k>=0; k-=(1<<m))
                    {
                        for(j=0; j<curimg->bm->w; j+=(1<<m))
                        {
                            l = k * curimg->bm->w + j;
                            fwrite(curimg->bm->data + l, 1, 1, ptr);
                        }
                    }
                }

                fwrite(&ncolors, sizeof(short), 1, ptr);
                for(j=0; j<256; j++)
                {
                    fwrite(&curimg->bm->palette[j].b, 1, 1, ptr);
                    fwrite(&curimg->bm->palette[j].g, 1, 1, ptr);
                    fwrite(&curimg->bm->palette[j].r, 1, 1, ptr);
                }
                fwrite(&ncolors, sizeof(short), 1, ptr);

                break;
            default:
                printf("Unrecognized texture type %d.\n", curimg->type);
                break;
        }
    }

    for(i=nimages, curfont=fonts; i<nfonts+nimages; i++, curfont++)
    {
        misc = 1;   
        fwrite(&curfont->w, sizeof(int), 1, ptr);
        fwrite(&curfont->h, sizeof(int), 1, ptr);
        fwrite(&misc, sizeof(int), 1, ptr);
        fwrite(&curfont->h, sizeof(int), 1, ptr);

        misc = 0;
        for(j=0, k=0; j<256; j++)
        {
            fwrite(&k, sizeof(short), 1, ptr);

            if(curfont->glyfs[j])
            {
                fwrite(&curfont->glyfs[j]->w, sizeof(short), 1, ptr);
                k += curfont->glyfs[j]->w;
            }
            else
                fwrite(&misc, sizeof(short), 1, ptr);
        }

        for(k=0; k<curfont->h; k++)
        {
            for(j=0; j<256; j++)
            {
                if(!curfont->glyfs[j])
                    continue;

                for(l=0; l<curfont->glyfs[j]->w; l++)
                    fwrite(&curfont->glyfs[j]->data[k * curfont->glyfs[j]->w + l], 1, 1, ptr);
            }
        }

        fwrite(&ncolors, sizeof(short), 1, ptr);
        for(j=0; j<256; j++)
        {
            fwrite(&curfont->palette[j].b, 1, 1, ptr);
            fwrite(&curfont->palette[j].g, 1, 1, ptr);
            fwrite(&curfont->palette[j].r, 1, 1, ptr);
        }
    }

    misc = 0;
    for(i=0, curimg=images; i<nimages; i++, curimg++)
    {
        fwrite(dataoffsets + i, sizeof(int), 1, ptr);
        fwrite(datasizes + i, sizeof(int), 1, ptr);
        fwrite(datasizes + i, sizeof(int), 1, ptr);
        fwrite(&curimg->type, 1, 1, ptr);
        fwrite(&misc, 1, 1, ptr);
        fwrite(&misc, 2, 1, ptr);
        fwrite(curimg->name, 1, 16, ptr);
    }

    for(i=nimages, curfont=fonts; i<nfonts+nimages; i++, curfont++)
    {
        fwrite(dataoffsets + i, sizeof(int), 1, ptr);
        fwrite(datasizes + i, sizeof(int), 1, ptr);
        fwrite(datasizes + i, sizeof(int), 1, ptr);
        misc = 0x46;
        fwrite(&misc, 1, 1, ptr);
        misc = 0;
        fwrite(&misc, 1, 1, ptr);
        fwrite(&misc, 2, 1, ptr);
        fwrite(curfont->name, 1, 16, ptr);
    }

    fclose(ptr);
}