#include "AssetManager.h"

#include <stdio.h>   
#include <stdlib.h> 
#include <string>
#include <limits>

GLuint AssetManager::getTexture(const char* texture, const char* source)
{
	// source:texture
	char* fullname = (char*) malloc(strlen(source) + 1 + strlen(texture) + 1);
	strcpy(fullname, source);
	strcat(fullname, ":");
	strcat(fullname, texture);

	for (int i = 0; i < strlen(fullname); i++)
		fullname[i] = tolower(fullname[i]);

	for (int i = 0; i < numtextures; i++)
	{
		if (!strcmp(texturelookup[i], fullname))
		{
			free(fullname);
			return texturenames[i];
		}
	}

	free(fullname);

	return (GLuint) UINT32_MAX;
}

GLuint AssetManager::setTexture(const char* texture, const char* source)
{
	// source:texture
	char* fullname = (char*)malloc(strlen(source) + 1 + strlen(texture) + 1);
	strcpy(fullname, source);
	strcat(fullname, ":");
	strcat(fullname, texture);

	for (int i = 0; i < strlen(fullname); i++)
		fullname[i] = tolower(fullname[i]);

	for (int i = 0; i < numtextures; i++)
	{
		if (!strcmp(texturelookup[i], fullname))
		{
			free(fullname);
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

	texturelookup[numtextures - 1] = (char*)malloc(strlen(fullname) + 1);
	strcpy(texturelookup[numtextures - 1], fullname);
	free(fullname);

	glGenTextures(1, &texturenames[numtextures - 1]);
	return texturenames[numtextures - 1];
}

void AssetManager::cleanup()
{
	for (int i = 0; i < numtextures; i++)
	{
		glDeleteTextures(1, (GLuint*) &i);
		free(texturelookup[i]);
	}

	free(texturelookup);
}