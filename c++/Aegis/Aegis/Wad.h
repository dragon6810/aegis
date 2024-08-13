#pragma once

#include <GL/glew.h>

#include <vector>
#include <string>
#include <stdio.h>

#include "defs.h"

class Wad
{
public:
    void CloseAll();
	void Unload();
	void Open(const char* filename);
	void LoadDecals(const char* filename);
	GLuint LoadTexture(const char* texturename);

	std::vector<std::string> textures;
    std::vector<FILE*> files;
};
