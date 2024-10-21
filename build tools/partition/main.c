//
//  main.c
//  Partition
//
//  Created by Henry Dunn on 9/16/24.
//

#include <stdio.h>

#include "map.h"

int main(int argc, char** argv)
{
    int i;
    
    if (argc == 1)
    {
        printf("USAGE: partition mapfile");
        exit(1);
    }

    for(i=1; i<argc-1; i++)
    {
        printf("%s\n", argv[i]);
    }
    
    LoadBrushSets(argv[i]);
    LoadEnts(argv[i]);
    ProcessWorld();
    WriteFile();
    
    return 0;
}
