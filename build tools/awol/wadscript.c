#include "wadscript.h"

FILE* ptr = 0;
ws_cmd_t *commands = 0;
ws_texture_t *textures = 0;
int ncommands = 0;
int ntextures = 0;

void WS_Open()
{
	ptr = fopen("wadfile", "r");
	if (!ptr)
	{
		printf("wadfile not found. Aborting.\n");
		exit(1);
	}
}

void WS_Parse()
{
	if (!ptr)
		return;
    
    while(WS_NextCMD());
}

boolean WS_NextCMD()
{
    int i;
    
    char c;
    long start, end;
    ws_cmd_t cmd;
    
    c = fgetc(ptr);
    while(c == ' ' || c == '\n' || c == '\t')
    {
        c = fgetc(ptr);
        if(feof(ptr))
            return false;
    }
    fseek(ptr, -1, SEEK_CUR);
    start = ftell(ptr);
    while((c = fgetc(ptr)) != ' ' && c != '\n' && c != '\t' && !feof(ptr));
    fseek(ptr, -1, SEEK_CUR);
    end = ftell(ptr);
    if(start == end)
        return false;
    cmd.cmd = calloc(end - start + 1, 1);
    fseek(ptr, start, SEEK_SET);
    for(i=0; i<end-start; i++)
        cmd.cmd[i] = fgetc(ptr);
    
    c = fgetc(ptr);
    while(c == ' ' || c == '\n' || c == '\t')
    {
        c = fgetc(ptr);
        if(feof(ptr))
            break;
    }
    fseek(ptr, -1, SEEK_CUR);
    start = ftell(ptr);
    while(!feof(ptr) && (c = fgetc(ptr)) != ' ' && c != '\n' && c != '\t');
    //fseek(ptr, -1, SEEK_CUR);
    end = ftell(ptr) - 1;
    if(start == end)
    {
        free(cmd.cmd);
        return false;
    }
    cmd.params = calloc(end - start + 1, 1);
    fseek(ptr, start, SEEK_SET);
    for(i=0; i<end-start; i++)
        cmd.params[i] = fgetc(ptr);
    
    if(commands)
        commands = realloc(commands, sizeof(ws_cmd_t) * (ncommands + 1));
    else
        commands = malloc(sizeof(ws_cmd_t) * (ncommands + 1));
    
    commands[ncommands++] = cmd;
    
    if(feof(ptr))
        return false;

    fseek(ptr, 1, SEEK_CUR);
    
    return true;
}

FILE* outfile;

void WS_Run()
{
    int i;
    
    for(i=ncommands-1; i>=0; i--)
    {
        if(!strcmp(commands[i].cmd, "outfile"))
        {
            outfile = fopen(commands[i].params, "wb");
            printf("Opening wad output \"%s\".\n", commands[i].params);
            if(outfile)
                break;
            else
                printf("Failed to open wad output \"%s\".\n", commands[i].params);
        }
    }
    
    for(i=0; i<ncommands; i++)
    {
        if(!strcmp(commands[i].cmd, "spraypaint"))
            WS_LoadBMP(commands[i].params, 0x40);
        else if(!strcmp(commands[i].cmd, "picture"))
            WS_LoadBMP(commands[i].params, 0x42);
        else if(!strcmp(commands[i].cmd, "texture"))
            WS_LoadBMP(commands[i].params, 0x43);
        else if(!strcmp(commands[i].cmd, "font"))
            WS_LoadBMP(commands[i].params, 0x46);
    }

    WS_WriteWAD();
}

boolean WS_LoadBMP(char* path, char type)
{
    int i, j;
    
    char* realpath;
    FILE* tptr;
    ws_texture_t tex;
    int x, y, outx, outy;
    
    char signature[2];
    uint32_t dataoffset;
    uint32_t w, h;
    uint16_t bitdepth;
    uint32_t compressiontype, ncolors;
    
    tex.type = type;

    if(strlen(path) > 15)
    {
        printf("Texture names \"%s\" is too long: max is 15 characters.\n", path);
        return false;
    }
    memset(tex.name, 0, sizeof(tex.name));
    strcpy(tex.name, path);
    
    realpath = calloc(strlen(path) + 4 + 1, 1);
    strcpy(realpath, path);
    realpath[strlen(realpath)] = '.';
    realpath[strlen(realpath)] = 'b';
    realpath[strlen(realpath)] = 'm';
    realpath[strlen(realpath)] = 'p';
    tptr = fopen(realpath, "rb");
    if(!tptr)
    {
        printf("File does not exist \"%s\".\n", realpath);
        free(realpath);
        return false;
    }
    
    printf("Loading Texture %s\n", realpath);
    fread(signature, 1, 2, tptr);
    if(signature[0] != 'B' || signature[1] != 'M')
    {
        printf("File \"%s\" is not a valid bitmap file.\n", realpath);
        free(realpath);
        fclose(tptr);
        return false;
    }
    fseek(tptr, 8, SEEK_CUR);
    fread(&dataoffset, sizeof(dataoffset), 1, tptr);
    fseek(tptr, 4, SEEK_CUR);
    fread(&w, sizeof(w), 1, tptr);
    fread(&h, sizeof(h), 1, tptr);
    tex.w = w;
    tex.h = h;
    fseek(tptr, 2, SEEK_CUR);
    fread(&bitdepth, sizeof(bitdepth), 1, tptr);
    if(bitdepth != 8)
    {
        printf("Bitmap \"%s\" does not use 8-bit pallete.\n", realpath);
        free(realpath);
        fclose(tptr);
        return false;
    }
    fread(&compressiontype, sizeof(compressiontype), 1, tptr);
    if(compressiontype != 0)
    {
        printf("Bitmap \"%s\" uses unsupported compression.\n", realpath);
        free(realpath);
        fclose(tptr);
        return false;
    }
    fseek(tptr, 12, SEEK_CUR);
    fread(&ncolors, sizeof(ncolors), 1, tptr);
    fseek(tptr, 4, SEEK_CUR);
    
    for(i=0; i<ncolors; i++)
    {
        fread(&tex.pallete[i], sizeof(rgb8_t), 1, tptr);
        fseek(tptr, 1, SEEK_CUR);
    }
    
    tex.pixeldata = malloc(w * h);
    for(y=h-1; y>=0; y--)
    {
        for(x=0; x<w; x++)
        {
            fread(tex.pixeldata + y * w + x, 1, 1, tptr);
        }
        
        fseek(tptr, w % 4, SEEK_CUR);
    }
    
    if(!textures)
        textures = malloc(sizeof(ws_texture_t) * (ntextures + 1));
    else
        textures = realloc(textures, sizeof(ws_texture_t) * (ntextures + 1));

    textures[ntextures++] = tex;
    
    free(realpath);
    fclose(tptr);
    
    return true;
}

boolean WS_WriteWAD()
{
    int i, j, x, y;
    ws_texture_t *curtex;

    char magic[4];
    int16_t ncolors = 256;
    char zero = 0;

    uint32_t datasize;
    uint32_t *texoffsets;
    uint32_t *texsizes;
    char p;

    if(!outfile)
    {
        printf("No valid wad output, aborting.\n");
        return false;
    }
    
    printf("Writing WAD...\n");
    
    magic[0] = 'W';
    magic[1] = 'A';
    magic[2] = 'D';
    magic[3] = '3';
    fwrite(magic, 1, 4, outfile);
    fwrite(&ntextures, sizeof(ntextures), 1, outfile);

    texoffsets = malloc(sizeof(uint32_t) * ntextures);
    texsizes = malloc(sizeof(uint32_t) * ntextures);
    for(i=0, curtex=textures, datasize=0; i<ntextures; i++, curtex++)
    {
        texoffsets[i] = datasize + 12;

        switch(curtex->type)
        {
            case 0x42:
                texsizes[i] = 10 + curtex->w * curtex->h + 256 * sizeof(rgb8_t);
                datasize += texsizes[i];
                break;
            case 0x43:
                texsizes[i] = 16 + 8 + 16 + 2 + 256 * sizeof(rgb8_t);
                for(j=0; j<4; j++)
                    texsizes[i] += (curtex->w >> j) * (curtex->h >> j);
                datasize += texsizes[i];
                break;
            case 0x46:
                break;
                texsizes[i] = 1042 + 256 * sizeof(rgb8_t) + curtex->w * curtex->h;
                datasize += texsizes[i];
                break;
        }
    }

    datasize += 12;
    fwrite(&datasize, sizeof(datasize), 1, outfile);
    datasize -= 12;

    for(i=0, curtex=textures; i<ntextures; i++, curtex++)
    {
        if(curtex->type == 0x46)
        {
            printf("Fonts not supported in this version of AWOL.\n");
            continue;
        }
        
        if(curtex->type == 0x43)
            fwrite(curtex->name, 1, sizeof(curtex->name), outfile);
        
        fwrite(&curtex->w, sizeof(curtex->w), 1, outfile);
        fwrite(&curtex->h, sizeof(curtex->h), 1, outfile);

        if(curtex->type == 0x42)
            fwrite(curtex->pixeldata, 1, curtex->w * curtex->h, outfile);
        else if(curtex->type == 0x43)
        {
            for(j=0, x=40; j<4; x+=((curtex->w * curtex->h) >> j), j++)
                fwrite(&x, sizeof(x), 1, outfile);

            for(j=0; j<4; j++)
            {
                for(y=0; y<curtex->h; y+=(1<<j))
                {
                    for(x=0; x<curtex->w; x+=(1<<j))
                    {
                        p = curtex->pixeldata[y * curtex->w + x];
                        fwrite(&p, 1, 1, outfile);
                    }
                }
            }
        }

        fwrite(&ncolors, sizeof(ncolors), 1, outfile);
        fwrite(curtex->pallete, sizeof(rgb8_t), 256, outfile);
    }

    for(i=0, curtex=textures; i<ntextures; i++, curtex++)
    {
        if(curtex->type == 0x46)
            continue;

        fwrite(&texoffsets[i], sizeof(uint32_t), 1, outfile);
        fwrite(&texsizes[i], sizeof(uint32_t), 1, outfile);
        fwrite(&texsizes[i], sizeof(uint32_t), 1, outfile);
        fwrite(&curtex->type, 1, 1, outfile);
        fwrite(&zero, 1, 1, outfile);
        fwrite(&zero, 1, 1, outfile);
        fwrite(&zero, 1, 1, outfile);
        fwrite(curtex->name, 1, sizeof(curtex->name), outfile);
    }

    free(texoffsets);
    free(texsizes);
}
