#include "AssetManager.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <string>
#include <limits>

GLuint AssetManager::getTexture(const char* texture, const char* source)
{
	// source:texture
	std::string fullname = std::string(source) + std::string(":") + std::string(texture);

	for (int i = 0; i < strlen(fullname.c_str()); i++)
		fullname[i] = tolower(fullname[i]);

	for (int i = 0; i < numtextures; i++)
	{
		if (texturelookup[i] == fullname.c_str())
		{
			return texturenames[i];
		}
	}

	return (GLuint) UINT32_MAX;
}

GLuint AssetManager::setTexture(const char* texture, const char* source)
{
	// source:texture
	std::string fullname = std::string(source) + std::string(":") + std::string(texture);

	for (int i = 0; i < strlen(fullname.c_str()); i++)
		fullname[i] = tolower(fullname[i]);

	for (int i = 0; i < numtextures; i++)
	{
		if (texturelookup[i] == fullname)
		{
			glDeleteTextures(1, &texturenames[i]);
			glGenTextures(1, &texturenames[i]);
			return texturenames[i];
		}
	}

	numtextures++;

	texturelookup.push_back("");
	texturenames.push_back(0);
	
	texturelookup[numtextures - 1] = fullname;

	glGenTextures(1, &texturenames[numtextures - 1]);

	return texturenames[numtextures - 1];
}

bool AssetManager::removeTexture(const char* texture, const char* source)
{
	std::string fullname = std::string(source) + std::string(":") + std::string(texture);

	for (int i = 0; i < strlen(fullname.c_str()); i++)
		fullname[i] = tolower(fullname[i]);

	for (int i = 0; i < numtextures; i++)
	{
		if (texturelookup[i] == fullname.c_str())
		{
			glDeleteTextures(1, &texturenames[i]);
			texturelookup.erase(texturelookup.begin() + i);
			texturenames.erase(texturenames.begin() + i);
			numtextures--;
			return true;
		}
	}

	return false;
}

void AssetManager::cleanup()
{
	for (int i = 0; i < numtextures; i++)
		glDeleteTextures(1, (GLuint*) &texturenames[i]);
}