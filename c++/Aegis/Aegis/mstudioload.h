#pragma once

#include "defs.h"

#define MSTUDIOMAXBONES 128
#define MSTUDIOMAXMESHVERTS 2048
#define MSTUDIOMAXBONECONTROLLERS 8

class mstudioload
{
	public:
		mstudioheader_t header;
		char* data;

		void load(const char* filename);

		~mstudioload();
};