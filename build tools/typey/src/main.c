#include <stdio.h>
#include <string.h>

#include "textfile.h"
#include "bitmap.h"

char cdtexture[64];
int margin, spacewidth;
int curx = 0;
bitmap_t *bm;

bitmap_t *glyphs[256];

void WriteGlyphs()
{
    int i;

    bitmap_t *curglyf;
    char* curfilename;

    curfilename = malloc(strlen(cdtexture) + 3 + 4 + 1);
    
    for(i=0; i<256; i++)
    {
        curglyf = glyphs[i];

        if(!curglyf)
            continue;

        memset(curfilename, 0, strlen(curfilename));
        sprintf(curfilename, "%s%d.bmp", cdtexture, i);
        WriteBitmap(curfilename, curglyf);
    }

    free(curfilename);
}

void LoadTexture(char* path)
{
    char *realpath;

    realpath = malloc(strlen(cdtexture) + strlen(path) + 4 + 1);
    strcpy(realpath, cdtexture);
    strcat(realpath, path);
    strcat(realpath, ".bmp");

    printf("Load Texture \"%s\".\n", realpath);

    bm = LoadBitmap(realpath);

    free(realpath);
}

void LoadGlyph(char* chars, int w)
{
    int i, x, y;

    bitmap_t *glyf;

    if(chars[0] == '`')
    {
        printf("curx: %d\nw:%d\nwidth:%d\n", curx, w, bm->w);
        printf("Loading glyphs \"%s\" with width %d.\n", chars, w);
    }

    if(curx + w - 1 > bm->w)
    {
        printf("curx: %d\n w:%d\n width:%d\n", curx, w, bm->w);
        printf("No space to load glyph \"%s\" with width %d.\n", chars, w);
        return;
    }

    glyf = malloc(sizeof(bitmap_t));
    for(i=0; i<strlen(chars); i++)
        glyphs[chars[i]] = glyf;

    glyf->w = w;
    glyf->h = bm->h;
    memcpy(glyf->palette, bm->palette, 256 * 3);
    glyf->data = malloc(glyf->w * glyf->h);

    for(i=0, y=bm->h-1; y>=0; y--)
    {
        for(x=curx; x<curx+w; x++, i++)
            glyf->data[i] = bm->data[y * bm->w + x];
    }

    curx += w + margin;
}

int main(int argc, char** argv)
{
    int i;
    char chars[64];
    int w;

    for(i=0; i<argc-1; i++)
    {
        
    }

    LoadScript(argv[i]);
    while(NextToken())
    {
        if(token[0] != '$')
            continue;

        if(!strcmp(token, "$cdtexture"))
        {
            if(!TokenAvailable())
                continue;

            NextToken();
            memcpy(cdtexture, token, 64);
            if(cdtexture[strlen(cdtexture)-1] != '/' && cdtexture[strlen(cdtexture)-1] != '\\')
                cdtexture[strlen(cdtexture)] = '/';
            continue;
        }
        if(!strcmp(token, "$margin"))
        {
            if(!TokenAvailable())
                continue;

            NextToken();
            sscanf(token, "%d", &margin);
            continue;
        }
        if(!strcmp(token, "$spacewidth"))
        {
            if(!TokenAvailable())
                continue;

            NextToken();
            sscanf(token, "%d", &spacewidth);

            glyphs[' '] = malloc(sizeof(bitmap_t));
            glyphs[' ']->w = spacewidth;
            glyphs[' ']->h = bm->h;
            memset(glyphs[' ']->palette, 0, 256 * 3);
            glyphs[' ']->data = malloc(spacewidth * bm->h);
            memset(glyphs[' ']->data, 0, spacewidth * bm->h);

            continue;
        }
        if(!strcmp(token, "$texture"))
        {
            if(!TokenAvailable())
                continue;

            NextToken();
            LoadTexture(token);
            continue;
        }
        if(!strcmp(token, "$glyphs"))
        {
            if(!TokenAvailable())
                continue;

            NextToken();
            memset(chars, 0, 64);
            strcpy(chars, token);

            if(!TokenAvailable())
                continue;

            NextToken();
            sscanf(token, "%d", &w);

            LoadGlyph(chars, w);
            continue;
        }
    }

    WriteGlyphs();
}