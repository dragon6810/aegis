#include <stdio.h>
#include <stdlib.h>

#include "TpkScript.h"

void printusage(FILE* stream)
{
    fprintf(stream, "usage:\n");
    fprintf(stream, "tpkgen scriptpath\n");
}

int main(int argc, char** argv)
{
    int i;

    TpkScript script;

    if(argc == 1)
    {
        printusage(stderr);
        exit(1);
    }

    for(i=1; i<argc-1; i++)
    {

    }

    if(!script.LoadScript(argv[i]))
        return 1;
    return !script.RunScript();
}