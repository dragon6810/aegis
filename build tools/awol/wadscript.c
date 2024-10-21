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
    cmd.cmd = malloc(end - start + 1);
    fseek(ptr, start, SEEK_SET);
    for(i=0; i<end-start; i++)
        cmd.cmd[i] = fgetc(ptr);
    cmd.cmd[i] = 0;
    
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
    fseek(ptr, -1, SEEK_CUR);
    end = ftell(ptr);
    cmd.params = malloc(end - start + 1);
    fseek(ptr, start, SEEK_SET);
    for(i=0; i<end-start; i++)
        cmd.params[i] = fgetc(ptr);
    cmd.params[i] = 0;
    
    if(commands)
        commands = realloc(commands, sizeof(ws_cmd_t) * (ncommands + 1));
    else
        commands = malloc(sizeof(ws_cmd_t) * (ncommands + 1));
    
    commands[ncommands++] = cmd;
    
    if(feof(ptr))
        return false;
    
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
            if(outfile)
                break;
        }
    }
    
    for(i=0; i<ncommands; i++)
    {
        if(!strcmp(commands[i].cmd, "texture"))
            WS_LoadBMP(commands[i].params, type);
    }
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
    
    if(strlen(path) > 16)
    {
        printf("Texture names \"%s\" is too long: max is 16 characters.\n", path);
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
            fread(tex.pixeldata + x * w + y, 1, 1, tptr);
        }
        
        fseek(tptr, w % 4, SEEK_CUR);
    }
    fread(tex.pixeldata, 1, w * h, tptr);
    
    if(!textures)
        textures = malloc(sizeof(ws_texture_t) * ntextures + 1);
    else
        textures = realloc(textures, malloc(sizeof(ws_texture_t) * ntextures + 1));
    
    textures[ntextures++] = tex;
    
    free(realpath);
    fclose(tptr);
    
    return true;
}

boolean WS_WriteWAD()
{
    if(!outfile)
        return false;
    
    
}
