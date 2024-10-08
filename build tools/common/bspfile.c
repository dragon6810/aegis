//
//  bspfile.c
//  Partition
//
//  Created by Henry Dunn on 10/8/24.
//

#include "bspfile.h"

#include <stdio.h>

void WriteBspFile(bspffile_t* bsp, char* file)
{
    const int version = 31;
    
    FILE* ptr;
    
    int entoffs;
    int planeoffs;
    int texoffs;
    int vertoffs;
    int visoffs;
    int nodeoffs;
    int texinfooffs;
    int faceoffs;
    int lightoffs;
    int clipoffs;
    int leafoffs;
    int marksurfoffs;
    
    
    ptr = fopen(file, "wb");
    if(!ptr)
        return;
    
    fwrite(&version, sizeof(version), 1, ptr);
    
    fclose(ptr);
}
