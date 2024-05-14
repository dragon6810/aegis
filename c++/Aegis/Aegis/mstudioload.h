#pragma once

#include "defs.h"

class mstudioload
{
	public:
		mstudioheader_t header;

		void load(const char* filename);
};