#include "map.h"

vec3_t* positions = 0;
int npositions = 0;

void LoadPositions(char* ents)
{
    char buff[1060];
    char* line;
    float pos[3];

    line = ents;
    while(sscanf(line, "%1059[^\n]", buff) && line < ents + strlen(ents))
    {
        line += strlen(buff) + 1;
        
        if(sscanf(buff, "\"origin\" \"%f %f %f\"\n", &pos[0], &pos[1], &pos[2]) != 3)
            continue;

        if(positions)
            positions = realloc(positions, sizeof(vec3_t) * (npositions + 1));
        else
            positions = malloc(sizeof(vec3_t) * (npositions + 1));

        positions[npositions][0] = pos[0];
        positions[npositions][1] = pos[1];
        positions[npositions][2] = pos[2];
        npositions++;
    }
}

void LoadEnts(char* file)
{
    FILE *ptr;
    uint32_t len;
    char* newfile;
    
    newfile = malloc(strlen(file) + 1 + 4);
    strcpy(newfile, file);
    strcat(newfile, ".ent");
    
    ptr = fopen(newfile, "r");
    if(!ptr)
    {
        printf("Failed to open entity file \"%s\"\n", newfile);
        free(newfile);
        return;
    }
    
    fseek(ptr, 0, SEEK_END);
    len = ftell(ptr);
    rewind(ptr);
    
    bspfile.entitydata = calloc(len + 1, 1);
    fread(bspfile.entitydata, len, 1, ptr);
    LoadPositions(bspfile.entitydata);
    
    free(newfile);
    fclose(ptr);
}