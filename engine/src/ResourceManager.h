#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define MAX_TEXTURES 1024

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

	static texture_t* FindTexture(std::string source, std::string id);
	static texture_t* NewTexture(void);
	static bool FreeTexture(texture_t* texture);
	static void UseTexture(texture_t* texture);
	static void AbandonTexture(texture_t* texture);
private:
	static texture_t textures[MAX_TEXTURES];
	static int lasttexture;
};