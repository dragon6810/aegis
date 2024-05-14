#include "binaryloader.h"

#include <iostream>
#include <fstream>

int loadBytes(const char* filename, char** data)
{
	std::ifstream is(filename, std::ifstream::binary);

    if (is)
    {
        is.seekg(0, is.end);
        int length = is.tellg();
        is.seekg(0, is.beg);

        *data = (char*) malloc(length);

        printf("Loading file %s: %d bytes.\n", filename, length);
        is.read(*data, length);

        is.close();

        return 0;
    }

    fprintf(stderr, "Failed to load file %s.\n", filename);

    return 1;
}