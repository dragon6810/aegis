#include "ResourceManager.h"

#include <c_string.h>

ResourceManager::texture_t ResourceManager::textures[MAX_TEXTURES]{};
int ResourceManager::lasttexture = 0;

ResourceManager::texture_t* ResourceManager::FindTexture(std::string source, std::string id)
{
	int i;
	texture_t* tex;

	for (i = 0, tex = textures; i < lasttexture; i++, tex++)
	{
		if (source == tex->source && id == tex->id)
			return tex;
	}

	return NULL;
}

ResourceManager::texture_t* ResourceManager::NewTexture()
{
	int i;
	
    for (i = 0; i < lasttexture; i++)
	{
		if (textures[i].name == -1)
			break;
	}

	if (i == lasttexture)
		lasttexture++;

	glGenTextures(1, &textures[i].name);
	return &textures[i];
}

bool ResourceManager::FreeTexture(texture_t* texture)
{
	int i;

	if(!texture)
		return false;

	i = (texture - textures) / sizeof(texture_t);

	if (i >= lasttexture)
		return false;

	if (i == lasttexture - 1)
		lasttexture--;

	textures[i].name = -1;
	return true;
}

void ResourceManager::UseTexture(texture_t* texture)
{
	if(!texture)
		return;

	texture->users++;
}

void ResourceManager::AbandonTexture(texture_t* texture)
{
	if(!texture)
		return;
	
	texture->users--;

	if (texture->users <= 0)
		FreeTexture(texture);
}
