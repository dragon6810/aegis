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
    exit(EXIT_FAILURE);

    return 1;
}

void LittleEndian(void* data, size_t size)
{
    short endiantest = 0x0001;
    char* endianessb = (char*)&endiantest;
    if (endianessb[0] == 1) // If the machine is little endian, abort. Nothing needs to be done
        return;

    uint8_t* bytes = (uint8_t*)data;
    for (size_t i = 0; i < (size >> 1); ++i)
    {
        uint8_t temp = bytes[i];
        bytes[i] = bytes[size - 1 - i];
        bytes[size - 1 - i] = temp;
    }
}

void BigEndian(void* data, size_t size)
{
    short endiantest = 0x0001;
    char* endianessb = (char*)&endiantest;
    if (endianessb[0] == 0) // If the machine is big endian, abort. Nothing needs to be done
        return;

    uint8_t* bytes = (uint8_t*)data;
    for (size_t i = 0; i < (size >> 1); ++i)
    {
        uint8_t temp = bytes[i];
        bytes[i] = bytes[size - 1 - i];
        bytes[size - 1 - i] = temp;
    }
}