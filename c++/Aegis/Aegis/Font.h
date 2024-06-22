#pragma once

#include <GL/glew.h>

#include <string>
#include <stdint.h>

#define FONT_NGLYPHS 256
#define FONT_ROWGLYPHS 16

typedef struct
{
	uint16_t startoffset;
	uint16_t charwidth;
} wadcharinfo_t;

typedef struct
{
	uint32_t width;
	uint32_t height;
	uint32_t rowcount;
	uint32_t rowheight;
} wadfont_t;

typedef struct
{
	uint32_t width;
	uint32_t height;
	uint32_t xoffset;
	uint32_t yoffset;
} glyph_t;

class Font
{
public:
	void Load(std::string font, std::string wad);

	int GetStringWidth(std::string text); // Get the width, in pixels of a string displayed in this font
	int GetHeight();

	int DrawGlyph(char c, int x, int y);
	int DrawString(std::string text, int x, int y);

	glyph_t glyphs[FONT_NGLYPHS];
	GLuint fonttex;
	uint32_t atlaswidth;
	uint32_t atlasheight;
};

