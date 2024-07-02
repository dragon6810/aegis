#include "TrueTypeFont.h"

#include <GL/glew.h>

#include "Game.h"

bool TrueTypeFont::Load(std::string name)
{
	int i;

	FILE* ptr;
	offsetsubtable_t offsettable;
	std::vector<tabledir_t> tabledirs;

	uint16_t glyfoffs;

	name = "deadbird/truetype/" + name;

	ptr = fopen(name.c_str(), "rb");
	if (ptr == nullptr)
		return false;

	fread(&offsettable, sizeof(offsettable), 1, ptr);
	SwapEndian(&offsettable.scalartype, sizeof(offsettable.scalartype));
	SwapEndian(&offsettable.numtables, sizeof(offsettable.numtables));
	SwapEndian(&offsettable.searchrange, sizeof(offsettable.searchrange));
	SwapEndian(&offsettable.entryselector, sizeof(offsettable.entryselector));
	SwapEndian(&offsettable.rangeshift, sizeof(offsettable.rangeshift));

	tabledirs.resize(offsettable.numtables);
	for (i = 0; i < offsettable.numtables; i++)
	{
		fread(&tabledirs[i], sizeof(tabledirs[0]), 1, ptr);
		SwapEndian(&tabledirs[i].checksum, sizeof(tabledirs[i].checksum));
		SwapEndian(&tabledirs[i].offset, sizeof(tabledirs[i].offset));
		SwapEndian(&tabledirs[i].length, sizeof(tabledirs[i].length));

		if (tabledirs[i].tag[0] == 'g' && tabledirs[i].tag[1] == 'l' && tabledirs[i].tag[2] == 'y' && tabledirs[i].tag[3] == 'f')
			glyfoffs = i;
	}

	fseek(ptr, tabledirs[glyfoffs].offset, SEEK_SET);
	LoadGlyph(ptr);

	fclose(ptr);
	return true;
}

void TrueTypeFont::SwapEndian(void* data, size_t size)
{
	uint8_t* bytes = (uint8_t*)data;
	for (size_t i = 0; i < size / 2; ++i) 
	{
		uint8_t temp = bytes[i];
		bytes[i] = bytes[size - 1 - i];
		bytes[size - 1 - i] = temp;
	}
}

void TrueTypeFont::LoadGlyph(FILE* ptr)
{
	glyphdesc_t glyfdesc;

	fread(&glyfdesc, sizeof(glyfdesc), 1, ptr);
	SwapEndian(&glyfdesc.ncontours, sizeof(glyfdesc.ncontours));
	SwapEndian(&glyfdesc.xmin, sizeof(glyfdesc.xmin));
	SwapEndian(&glyfdesc.ymin, sizeof(glyfdesc.ymin));
	SwapEndian(&glyfdesc.xmax, sizeof(glyfdesc.xmax));
	SwapEndian(&glyfdesc.ymax, sizeof(glyfdesc.ymax));

	if (glyfdesc.ncontours < 0)
	{
		glyfdesc.ncontours = -glyfdesc.ncontours;
		LoadCompoundGlyph(ptr, glyfdesc);
	}
	else
	{
		LoadSimpleGlyph(ptr, glyfdesc);
	}
}

void TrueTypeFont::LoadSimpleGlyph(FILE* ptr, glyphdesc_t desc)
{
	int i;
	int j;

	std::vector<uint16_t> contourends;
	uint16_t instructionlength;
	std::vector<ubyte_t> instructions;

	uint32_t numpoints;
	std::vector<ubyte_t> flags;
	ubyte_t numrepeats;

	std::vector<int16_t> xcoords;
	std::vector<int16_t> ycoords;

	glyph_t glyf;
	std::vector<vec2_t> points;

	contourends.resize(desc.ncontours);
	glyf.contourendindices.resize(desc.ncontours);
	for (i = 0; i < contourends.size(); i++)
	{
		fread(&contourends[i], sizeof(contourends[i]), 1, ptr);
		SwapEndian(&contourends[i], sizeof(contourends[i]));
		glyf.contourendindices[i] = contourends[i];
	}

	fread(&instructionlength, sizeof(instructionlength), 1, ptr);
	SwapEndian(&instructionlength, sizeof(instructionlength));
	instructions.resize(instructionlength);
	fread(instructions.data(), 1, instructions.size(), ptr);

	numpoints = contourends[contourends.size() - 1] + 1;
	flags.resize(numpoints);
	for (i = 0; i < numpoints; i++)
	{
		fread(&flags[i], 1, 1, ptr);

		if (flags[i] & 0x08)
		{
			fread(&numrepeats, sizeof(numrepeats), 1, ptr);
			for(j = 0; j < numrepeats; j++)
				flags[++i] = flags[i - 1];
		}
	}

	points.resize(numpoints);

	xcoords.resize(numpoints);
	for (i = 0; i < numpoints; i++)
	{
		if (flags[i] & 0x02)
		{
			if (i != 0)
				xcoords[i] = xcoords[i - 1];

			int val;
			ubyte_t b;
			fread(&b, 1, 1, ptr);
			val = b;

			if (flags[i] & 0x10)
				val = -val;

			xcoords[i] += val;
		}
		else
		{
			if (i != 0)
				xcoords[i] = xcoords[i - 1];

			if(!(flags[i] & 0x10))
			{
				int16_t val;
				fread(&val, sizeof(val), 1, ptr);
				SwapEndian(&val, sizeof(val));

				xcoords[i] += val;
			}
		}

		points[i].x = xcoords[i];
	}

	ycoords.resize(numpoints);
	for (i = 0; i < numpoints; i++)
	{
		if (flags[i] & 0x04)
		{
			if (i != 0)
				ycoords[i] = ycoords[i - 1];

			int val;
			ubyte_t b;
			fread(&b, 1, 1, ptr);
			val = b;

			if (flags[i] & 0x20)
				val = -val;

			ycoords[i] += val;
		}
		else
		{
			if (i != 0)
				ycoords[i] = ycoords[i - 1];

			if (!(flags[i] & 0x20))
			{
				int16_t val;
				fread(&val, sizeof(val), 1, ptr);
				SwapEndian(&val, sizeof(val));

				ycoords[i] += val;
			}
		}

		points[i].y = ycoords[i];
	}
	
	glyf.desc = desc;
	glyf.points = points;
	glyfs.push_back(glyf);
}

void TrueTypeFont::LoadCompoundGlyph(FILE* ptr, glyphdesc_t desc)
{

}

void TrueTypeFont::DrawDebug()
{
	int i;
	int j;
	
	int cstart;
	int npoints;
	std::vector<vec2_t> points;

	int g;

	g = 0;

	glColor3f(1, 0, 1);

	glBegin(GL_LINES);
	cstart = 0;
	for (i = 0; i < glyfs[g].contourendindices.size(); i++)
	{
		npoints = glyfs[g].contourendindices[i] - cstart + 1;
		points.clear();
		for (j = cstart; j < cstart + npoints; j++)
		{
			vec2_t p = glyfs[g].points[j] * (1.0 / 10.0);
			p.x = p.x + SCREEN_MED_WIDTH / 2;
			p.y = p.y + SCREEN_MED_HEIGHT / 2;
			points.push_back(p);
		}

		for (j = 0; j < points.size(); j++)
		{
			vec2_t p1 = points[j];
			vec2_t p2 = points[(j + 1) % points.size()];

			glVertex2f(p1.x, p1.y);
			glVertex2f(p2.x, p2.y);
		}

		cstart += npoints;
	}
	glEnd();

	glPointSize(5.0);
	glBegin(GL_POINTS);
	for (i = 0; i < glyfs[g].points.size(); i++)
	{
		vec2_t p = glyfs[g].points[i] * (1.0 / 10.0);
		p.x = p.x + SCREEN_MED_WIDTH / 2;
		p.y = p.y + SCREEN_MED_HEIGHT / 2;
		glVertex2f(p.x, p.y);
	}
	glEnd();
	glColor3f(1, 1, 1);
}