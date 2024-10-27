#include "ResourceManager.h"

ResourceManager::texture_t* ResourceManager::FindTexture(std::string source, std::string id)
{
	int i;
	texture_t* tex;

	for (i = 0, tex = textures.data(); i < textures.size(); i++, tex++)
	{
		if (source == tex->source && id == tex->id)
			return tex;
	}

	return NULL;
}

ResourceManager::texture_t* ResourceManager::NewTexture()
{
	textures.push_back({});
	return &textures[textures.size() - 1];
}

bool ResourceManager::FreeTexture(texture_t* texture)
{
	int i;

	i = (texture - textures.data()) / sizeof(texture_t);

	if (i >= textures.size())
		return false;

	textures.erase(textures.begin() + i);
	return true;
}

void ResourceManager::UseTexture(texture_t* texture)
{
	texture->users++;
}

void ResourceManager::AbandonTexture(texture_t* texture)
{
	texture->users--;

	if (texture->users <= 0)
		FreeTexture(texture);
}