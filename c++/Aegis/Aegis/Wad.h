#pragma once

#include "defs.h"

class Wad
{
public:
	void Load(const char* filename);
	void LoadDecals(const char* filename);

	wadheader_t* whdr;
};

