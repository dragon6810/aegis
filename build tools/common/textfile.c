//
//  textfile.c
//  Brushstroke
//
//  Created by Henry Dunn on 8/22/24.
//

#include "textfile.h"

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
