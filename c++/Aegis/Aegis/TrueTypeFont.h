#pragma once

#include <stdio.h>

#include <string>
#include <vector>

#include "defs.h"

class TrueTypeFont
{
private:
#pragma pack(push, 1)
	struct glyphdesc_t
	{
		int16_t ncontours;
		int16_t xmin;
		int16_t ymin;
		int16_t xmax;
		int16_t ymax;
	};
public:
	struct glyph_t
	{
		glyphdesc_t desc;
		std::vector<int> contourendindices;
		std::vector<vec2_t> points;
	};

	bool Load(std::string name);

	void DrawDebug();
private:
	#pragma pack(push, 1)
	struct offsetsubtable_t
	{
		uint32_t scalartype;
		uint16_t numtables;
		uint16_t searchrange;
		uint16_t entryselector;
		uint16_t rangeshift;
	};

	#pragma pack(push, 1)
	struct tabledir_t
	{
		char tag[4];
		uint32_t checksum;
		uint32_t offset;
		uint32_t length;
	};

	std::vector<glyph_t> glyfs;

	static void SwapEndian(void* data, size_t size);

	void LoadGlyph(FILE* ptr);
	void LoadSimpleGlyph(FILE* ptr, glyphdesc_t desc);
	void LoadCompoundGlyph(FILE* ptr, glyphdesc_t desc);
};

