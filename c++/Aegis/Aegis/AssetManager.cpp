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
		if (!strcmp(texturelookup[i], fullname.c_str()))
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
		if (!strcmp(texturelookup[i], fullname.c_str()))
		{
			glDeleteTextures(1, &texturenames[i]);
			glGenTextures(1, &texturenames[i]);
			return texturenames[i];
		}
	}

	if (++numtextures == 1)
	{
		texturelookup = (char**)malloc(sizeof(char*));
		texturenames = (GLuint*)malloc(sizeof(GLuint));
	}
	else
	{
		texturelookup = (char**)realloc(texturelookup, numtextures * sizeof(char*));
		texturenames = (GLuint*)realloc(texturenames, numtextures * sizeof(GLuint));
	}

	texturelookup[numtextures - 1] = (char*)malloc(strlen(fullname.c_str()) + 1);
	strcpy(texturelookup[numtextures - 1], fullname.c_str());

	glGenTextures(1, &texturenames[numtextures - 1]);

	return texturenames[numtextures - 1];
}

void AssetManager::cleanup()
{
	for (int i = 0; i < numtextures; i++)
	{
		glDeleteTextures(1, (GLuint*) &texturenames[i]);
		free(texturelookup[i]);
	}

	free(texturenames);
	free(texturelookup);
}