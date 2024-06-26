#pragma once

#include <GL/glew.h>

#include "defs.h"

class Wad
{
public:
	void Load(const char* filename);
	void LoadDecals(const char* filename);
	static GLuint LoadTexture(const char* filename, const char* texturename);

	wadheader_t* whdr;
};

