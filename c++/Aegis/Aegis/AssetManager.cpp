#include "AssetManager.h"

#include <stdio.h>   
#include <stdlib.h> 
#include <string>
#include <GL/glew.h>

int AssetManager::getTextureIndex(char* texture, char* source)
{
	// source:texture
	char* fullname = (char*) malloc(strlen(source) + 1 + strlen(texture) + 1);
	strcpy(fullname, source);
	strcat(fullname, ":");
	strcat(fullname, texture);

	for (int i = 0; i < numtextures; i++)
	{
		if (!strcmp(texturenames[i], fullname))
		{
			free(fullname);
			return i;
		}
	}

	if (++numtextures == 1)
		texturenames = (char**) malloc(sizeof(char*));
	else
		texturenames = (char**) realloc(texturenames, numtextures * sizeof(char*));
	texturenames[numtextures - 1] = (char*) malloc(strlen(fullname) + 1);
	strcpy(texturenames[numtextures - 1], fullname);
	free(fullname);
	return -(numtextures);
}

void AssetManager::cleanup()
{
	for (int i = 0; i < numtextures; i++)
	{
		glDeleteTextures(1, (GLuint*) &i);
		free(texturenames[i]);
	}

	free(texturenames);
}