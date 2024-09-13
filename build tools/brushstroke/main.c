//
//  main.c
//  Brushstroke
//
//  Created by Henry Dunn on 8/21/24.
//

#include <stdio.h>

#include "mapfile.h"

int main(int argc, char** argv)
{
    int i;
    
    printf("[========== BRUSHSTROKE v0.1 ==========]\n");
    printf("Brushstroke is under the MIT license.\n\n");
    
    for(i=0; i<argc-1; i++)
    {
        if(!strcmp(argv[i], "-h"))
        {
            ReadHullSpec(argv[++i]);
            continue;
        }
    }
    
    ParseMap(argv[i]);
    
    return 0;
}
