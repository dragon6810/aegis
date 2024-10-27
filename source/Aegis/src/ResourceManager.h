#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>

class ResourceManager
{
public:
	typedef struct
	{
		GLuint name;
		std::string id;
		std::string source;
		uint32_t users;

		uint32_t width;
		uint32_t height;
	} texture_t;

	texture_t* FindTexture(std::string source, std::string id);
	texture_t* NewTexture();
	bool FreeTexture(texture_t* texture);
	void UseTexture(texture_t* texture);
	void AbandonTexture(texture_t* texture);
private:
	std::vector<texture_t> textures;
};