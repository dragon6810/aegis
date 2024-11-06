//
//  textfile.c
//  Brushstroke
//
//  Created by Henry Dunn on 8/22/24.
//

#include "textfile.h"

#include <string.h>

void GetLine(char** out, FILE* ptr)
{
    int i;
    
    int len;
    long start;
    char c;
    
    len = 1;
    start = ftell(ptr);
    while(fgetc(ptr) != '\n' && !feof(ptr))
        len++;
    
    fseek(ptr, start, SEEK_SET);
    *out = (char*) malloc(len);
    for(i=0; i<len; i++)
        (*out)[i] = fgetc(ptr);
    
    (*out)[i-1] = 0;
}

char token[64];
char *script, *scriptp, *scriptend;
int scriptline;

void LoadScript(char* filename)
{
    FILE* ptr;
    int len;

    ptr = fopen(filename, "r");
    if (!ptr)
    {
        printf("Can't find script \"%s\".\n", filename);
        return;
    }
    
    fseek(ptr, 0, SEEK_END);
    len = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);

    scriptp = script = malloc(len);
    fread(scriptp, 1, len, ptr);
    scriptend = script + len;
    scriptline = 1;
    fclose(ptr);
}

boolean NextToken()
{
    int i;

    memset(token, 0, sizeof(token));

    if (scriptp >= scriptend)
        return false;

    while (*scriptp <= 32)
    {
        if (scriptp >= scriptend)
            return false;

        if (*scriptp++ == '\n')
            scriptline++;
    }

    if (scriptp >= scriptend)
        return false;

    if (*scriptp == '/' && *(scriptp+1) == '/')
    {
        while (*scriptp++ != '\n')
        {
            if (scriptp >= scriptend)
                return false;
        }
    }

    i = 0;
    while (*scriptp > 32 && scriptp < scriptend && i < 63)
    {
        token[i] = *scriptp++;
        i++;
    }

    return true;
}

boolean TokenAvailable()
{
    char* searchp;

    searchp = scriptp;

    if (searchp >= scriptend)
        return false;

    while (*searchp <= 32)
    {
        if (searchp >= scriptend)
            return false;

        if (*searchp++ == '\n')
            return false;
    }

    if (searchp >= scriptend)
        return false;

    if (*scriptp == '/' && *(scriptp+1) == '/')
    {
        return false;
    }

    return true;
}

char* DefaultExtension(char* name, char* ext)
{
    int i;
    char *new;

    if(!strcmp(name + strlen(name) - strlen(ext), ext))
        return name;

    new = malloc(strlen(name) + strlen(ext) + 1);
    strcpy(new, name);
    strcat(new, ext);
    
    return new;
}