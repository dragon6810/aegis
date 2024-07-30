#pragma once

#include <GL/glew.h>

#include <vector>
#include <string>

#include "defs.h"

class Wad
{
public:
	void Unload();
	void Load(const char* filename);
	void LoadDecals(const char* filename);
	GLuint LoadTexture(const char* filename, const char* texturename);

	wadheader_t* whdr;

	std::vector<std::string> textures;
};