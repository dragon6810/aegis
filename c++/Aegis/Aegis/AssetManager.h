#pragma once

#include <unordered_map>
#include <string>

#include <GL/glew.h>

class AssetManager
{
public:
	static AssetManager& getInst()
	{
		static AssetManager instance;
		return instance;
	}

	// Source would be the name of the mdl, wad, bsp, etc.
	GLuint getTexture(const char* texture, const char* source);
	GLuint setTexture(const char* texture, const char* source);

	void cleanup();
private:
	AssetManager() {}

	AssetManager(AssetManager const&);
	void operator=(AssetManager const&);

	std::vector<std::string> texturelookup;
	std::vector<GLuint> texturenames;
	int numtextures = 0;
};

