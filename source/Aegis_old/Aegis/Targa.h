#pragma once

#include <GL/glew.h>

#include <string>

class Targa
{
public:
	static GLuint LoadTargaImage(std::string path, int* width, int* height);
private:
	enum TargaImageTypes
	{
		TGA_IMG_NONE = 0,
		TGA_IMG_UNCOMP_CMAP = 1,
		TGA_IMG_UNCOMP_TRUE = 2,
		TGA_IMG_UNCOMP_MONO = 3,
		TGA_IMG_RLE_CMAP = 9,
		TGA_IMG_RLE_TRUE = 10,
		TGA_IMG_RLE_MONO = 11,
	};
};

