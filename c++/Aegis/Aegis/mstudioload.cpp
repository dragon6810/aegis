#include "mstudioload.h"

#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "binaryloader.h"

void mstudioload::load(const char* filename)
{
	data = nullptr;
	loadBytes(filename, &data);

	header = *((mstudioheader_t*)data);

	if (header.id[0] != 'I' || header.id[1] != 'D' || header.id[2] != 'S' || header.id[3] != 'T')
	{
		printf("Unrecognized studio model id %c%c%c%c on model %s. Expect IDST.\n", header.id[0], header.id[1], header.id[2], header.id[3], header.name);
		return;
	}

	if (header.version != 10)
	{
		printf("Unrecognized studio model version %d on model %s. Expect 10.\n", header.version, header.name);
		return;
	}
}

mstudioload::~mstudioload()
{
	free(data);
}