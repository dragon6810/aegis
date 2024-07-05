#pragma once

#include <stdio.h>

#include <string>
#include <vector>
#include <array>
#include <unordered_map>

#include "defs.h"

class TrueTypeFont
{
private:
	typedef int fixed32_t;

#pragma pack(push, 1)
	struct hdr_t
	{
		fixed32_t version;
		fixed32_t revision;
		uint32_t checksumadj;
		uint32_t magic;       // 0x5F0F3CF5
		uint16_t flags;
		uint16_t emsize;
		int64_t createddate;
		int64_t modifieddate;
		int16_t xmin;
		int16_t ymin;
		int16_t xmax;
		int16_t ymax;
		uint16_t macstyle;
		uint16_t smallestsize;
		int16_t directionhint;
		int16_t locaformat;
		int16_t glyphformat;
	};

#pragma pack(push, 1)
	struct hhdr_t
	{
		fixed32_t version;
		int16_t ascent;
		int16_t descent;
		int16_t linegap;
		uint16_t maxadvw;
		int16_t minleftbear;
		int16_t minrightbear;
		int16_t maxxextend;
		int16_t caretrise;
		int16_t caretrun;
		int16_t caretoffset;  // 0 for no slant
		int16_t reserved[4];  // Always 0
		int16_t datafmt;	  // ??
		uint16_t nhmetrics;   // Number of horizontal metrics in the hmtx table
	};

#pragma pack(push, 1)
	struct maxp_t
	{
		fixed32_t version;
		uint16_t numglyphs;
		uint16_t maxpoints;
		uint16_t maxcontours;
		uint16_t maxcomponentpoints;
		uint16_t maxcomponentcontours;
		uint16_t maxzones;
		uint16_t maxtwilightpoints;
		uint16_t maxstorage;
		uint16_t maxfunctiondefs;
		uint16_t maxinstructiondefs;
		uint16_t maxstackelements;
		uint16_t maxinstructionsize;
		uint16_t maxcomponentel;
		uint16_t maxcomponentdepth;
	};

#pragma pack(push, 1)
	struct glyphdesc_t
	{
		int16_t ncontours;
		int16_t xmin;
		int16_t ymin;
		int16_t xmax;
		int16_t ymax;
	};

	struct line_t
	{
		uint32_t p0;
		uint32_t p1;
	};

	struct bezier_t
	{
		uint32_t p0;
		uint32_t p1;
		uint32_t p2;
	};
public:
	struct trimesh_t
	{
		std::vector<vec2_t> points;
		std::vector<int> indices;
	};

	struct glyph_t
	{
		glyphdesc_t desc;
		uint16_t advw;
		int16_t leftbear;
		std::vector<vec2_t> points;
		std::vector<ubyte_t> flags;
		std::vector<line_t> lines;
		std::vector<bezier_t> beziers;

		std::vector<std::array<int, 3>> triangles;
		std::vector<std::vector<vec2_t>> bezierfans;
	};

	bool Load(std::string name);

	int DrawString(std::string txt, float x, float y, float scale);
	int DrawCenteredString(std::string txt, float x, float y, float scale);
	int StringWidth(std::string txt, float scale);

	static trimesh_t EarClip(std::vector<vec2_t> contours, std::vector<int> contourends);
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

#pragma pack(push, 1)
	struct cmap_t
	{
		uint16_t version;
		uint16_t nsubtables;
	};

#pragma pack(push, 1)
	struct cmapst_t
	{
		uint16_t platid;
		uint16_t platspec;
		uint32_t offset;
	};

	enum cmap_platforms
	{
		CPLAT_UTF = 0,
		CPLAT_MAC = 1,
		CPLAT_MS = 3,
	};

	enum utf_modes
	{
		UTF_1_0 = 0,
		UTF_1_1 = 1,
		UTF_93 = 2,			 // Deprecated
		UTF_2_0_BMP = 3,
		UTF_2_0 = 4,
		UTF_VAR = 5,		 // ??
		UTF_LAST_RESORT = 6, // ??
	};

	hdr_t hdr;
	maxp_t maxp;
	hhdr_t hhdr;
	std::unordered_map<wchar_t, int> cmap;
	std::vector<glyph_t> glyfs;

	int cmapformat;
	int platformspec;

	uint32_t IndexCMap(wchar_t c);

	static void SwapEndian(void* data, size_t size);
	static void DrawBezier(vec2_t p0, vec2_t p1, vec2_t p2);

	void LoadCMap(FILE* ptr);
	// MUST be called after glyphs are generated AND after loading the horizontal header (hhdr)
	void LoadHmtx(FILE* ptr);
	void LoadGlyph(FILE* ptr);
	void LoadSimpleGlyph(FILE* ptr, glyphdesc_t desc);
	void LoadCompoundGlyph(FILE* ptr, glyphdesc_t desc);

	int DrawGlyph(wchar_t c, float x, float y, float scale);
	int GlyphWidth(wchar_t c, float scale);
};