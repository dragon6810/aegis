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
    
    nhulls = 4;
    
    for(i=0; i<argc-1; i++)
    {
        // TODO: Parse arguments
    }
    
    ParseMap(argv[i]);
    
    return 0;
}
