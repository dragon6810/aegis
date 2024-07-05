#include "TrueTypeFont.h"

#include <GL/glew.h>

#include <string>

#include "Print.h"

#include "mathutils.h"

#include "Game.h"

bool TrueTypeFont::Load(std::string name)
{
	int i;
	FILE* ptr;
	offsetsubtable_t offsettable;
	std::vector<tabledir_t> tabledirs;
	std::unordered_map<std::string, int> tagdirs;
	std::vector<uint32_t> localengths;
	std::vector<uint32_t> locaoffsets;

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

		std::string tag(4, 0);
		tag[0] = tabledirs[i].tag[0];
		tag[1] = tabledirs[i].tag[1];
		tag[2] = tabledirs[i].tag[2];
		tag[3] = tabledirs[i].tag[3];
		tagdirs[tag] = i;
	}

	fseek(ptr, tabledirs[tagdirs["cmap"]].offset, SEEK_SET);
	LoadCMap(ptr);
	
	int locaoffsetsize;
	fseek(ptr, tabledirs[tagdirs["head"]].offset, SEEK_SET);
	fread(&hdr, sizeof(hdr), 1, ptr);
	SwapEndian(&hdr.version, sizeof(hdr.version));
	SwapEndian(&hdr.revision, sizeof(hdr.revision));
	SwapEndian(&hdr.checksumadj, sizeof(hdr.checksumadj));
	SwapEndian(&hdr.magic, sizeof(hdr.magic));
	SwapEndian(&hdr.flags, sizeof(hdr.flags));
	SwapEndian(&hdr.emsize, sizeof(hdr.emsize));
	SwapEndian(&hdr.createddate, sizeof(hdr.createddate));
	SwapEndian(&hdr.modifieddate, sizeof(hdr.modifieddate));
	SwapEndian(&hdr.xmin, sizeof(hdr.xmin));
	SwapEndian(&hdr.ymin, sizeof(hdr.ymin));
	SwapEndian(&hdr.xmax, sizeof(hdr.xmax));
	SwapEndian(&hdr.ymax, sizeof(hdr.ymax));
	SwapEndian(&hdr.macstyle, sizeof(hdr.macstyle));
	SwapEndian(&hdr.smallestsize, sizeof(hdr.smallestsize));
	SwapEndian(&hdr.directionhint, sizeof(hdr.directionhint));
	SwapEndian(&hdr.locaformat, sizeof(hdr.locaformat));
	SwapEndian(&hdr.glyphformat, sizeof(hdr.glyphformat));
	locaoffsetsize = hdr.locaformat ? 4 : 2;

	fseek(ptr, tabledirs[tagdirs["maxp"]].offset, SEEK_SET);
	fread(&maxp, sizeof(maxp), 1, ptr);
	SwapEndian(&maxp.version, sizeof(maxp.version));
	SwapEndian(&maxp.numglyphs, sizeof(maxp.numglyphs));
	SwapEndian(&maxp.maxpoints, sizeof(maxp.maxpoints));
	SwapEndian(&maxp.maxcontours, sizeof(maxp.maxcontours));
	SwapEndian(&maxp.maxcomponentpoints, sizeof(maxp.maxcomponentpoints));
	SwapEndian(&maxp.maxcomponentcontours, sizeof(maxp.maxcomponentcontours));
	SwapEndian(&maxp.maxzones, sizeof(maxp.maxzones));
	SwapEndian(&maxp.maxtwilightpoints, sizeof(maxp.maxtwilightpoints));
	SwapEndian(&maxp.maxstorage, sizeof(maxp.maxstorage));
	SwapEndian(&maxp.maxfunctiondefs, sizeof(maxp.maxfunctiondefs));
	SwapEndian(&maxp.maxinstructiondefs, sizeof(maxp.maxinstructiondefs));
	SwapEndian(&maxp.maxstackelements, sizeof(maxp.maxstackelements));
	SwapEndian(&maxp.maxinstructionsize, sizeof(maxp.maxinstructionsize));
	SwapEndian(&maxp.maxcomponentel, sizeof(maxp.maxcomponentel));
	SwapEndian(&maxp.maxcomponentdepth, sizeof(maxp.maxcomponentdepth));

	fseek(ptr, tabledirs[tagdirs["hhea"]].offset, SEEK_SET);
	fread(&hhdr, sizeof(hhdr), 1, ptr);
	SwapEndian(&hhdr.version, sizeof(hhdr.version));
	SwapEndian(&hhdr.ascent, sizeof(hhdr.ascent));
	SwapEndian(&hhdr.descent, sizeof(hhdr.descent));
	SwapEndian(&hhdr.linegap, sizeof(hhdr.linegap));
	SwapEndian(&hhdr.maxadvw, sizeof(hhdr.maxadvw));
	SwapEndian(&hhdr.minleftbear, sizeof(hhdr.minleftbear));
	SwapEndian(&hhdr.minrightbear, sizeof(hhdr.minrightbear));
	SwapEndian(&hhdr.maxxextend, sizeof(hhdr.maxxextend));
	SwapEndian(&hhdr.caretrise, sizeof(hhdr.caretrise));
	SwapEndian(&hhdr.caretrun, sizeof(hhdr.caretrun));
	SwapEndian(&hhdr.caretoffset, sizeof(hhdr.caretoffset));
	SwapEndian(&hhdr.datafmt, sizeof(hhdr.datafmt));
	SwapEndian(&hhdr.nhmetrics, sizeof(hhdr.nhmetrics));

	localengths.resize(maxp.numglyphs + 1);
	locaoffsets.resize(maxp.numglyphs + 1);

	fseek(ptr, tabledirs[tagdirs["loca"]].offset, SEEK_SET);
	for (i = 0; i < maxp.numglyphs + 1; i++)
	{
		uint32_t offset;
		if (locaoffsetsize == 2)
		{
			uint16_t offs;
			fread(&offs, sizeof(offs), 1, ptr);
			SwapEndian(&offs, sizeof(offs));
			offset = offs;
		}
		else
		{
			uint32_t offs;
			fread(&offs, sizeof(offs), 1, ptr);
			SwapEndian(&offs, sizeof(offs));
			offset = offs;
		}

		locaoffsets[i] = offset;
	}

	for (i = 0; i < maxp.numglyphs - 1; i++)
	{
		fseek(ptr, tabledirs[tagdirs["glyf"]].offset + (locaoffsets[i] << 1), SEEK_SET);
		LoadGlyph(ptr);
	}

	fseek(ptr, tabledirs[tagdirs["hmtx"]].offset, SEEK_SET);
	LoadHmtx(ptr);

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

void TrueTypeFont::DrawBezier(vec2_t p0, vec2_t p1, vec2_t p2)
{
	const int res = 16;

	int i;

	float tstep = 1.0 / (float) res;

	glBegin(GL_LINES);

	float t = 0.0;
	for (i = 0; i < res; i++)
	{
		vec2_t a = Vector2Bezier(p0, p1, p2, t);
		vec2_t b = Vector2Bezier(p0, p1, p2, t + tstep);

		glVertex2f(a.x, a.y);
		glVertex2f(b.x, b.y);

		t += tstep;
	}

	glEnd();
}

uint32_t TrueTypeFont::IndexCMap(wchar_t c)
{
	return cmap[c];
}

void TrueTypeFont::LoadCMap(FILE* ptr)
{
	int i;
	uint32_t cmapoffs;

	std::vector<uint16_t> segstartcodes;
	std::vector<uint16_t> segendcodes;
	std::vector<uint16_t> segdeltas;
	std::vector<uint16_t> segrangeoffsets;

	cmap_t hdr;
	std::vector<cmapst_t> subtables;

	cmapoffs = ftell(ptr);

	fread(&hdr, sizeof(hdr), 1, ptr);
	SwapEndian(&hdr.version, sizeof(hdr.version));
	SwapEndian(&hdr.nsubtables, sizeof(hdr.nsubtables));

	subtables.resize(hdr.nsubtables);
	for (i = 0; i < hdr.nsubtables; i++)
	{
		fread(&subtables[i], sizeof(subtables[i]), 1, ptr);
		SwapEndian(&subtables[i].platid, sizeof(subtables[i].platid));
		SwapEndian(&subtables[i].platspec, sizeof(subtables[i].platspec));
		SwapEndian(&subtables[i].offset, sizeof(subtables[i].offset));
	}

	for (i = 0; i < hdr.nsubtables; i++)
	{
		if (subtables[i].platid != CPLAT_UTF) // I only care about UTF
			continue;

		if ((subtables[i].platspec != UTF_2_0_BMP) && (subtables[i].platspec != UTF_2_0))
			continue;

		fseek(ptr, cmapoffs + subtables[i].offset, SEEK_SET);
		uint16_t version;
		fread(&version, sizeof(version), 1, ptr);
		SwapEndian(&version, sizeof(version));
		cmapformat = version;
		platformspec = subtables[i].platspec;

		if (version == 4)
		{
			uint16_t len;
			uint16_t glyfidlen = 0;
			uint16_t segcount;

			int rangepos;

			fread(&len, sizeof(len), 1, ptr);
			SwapEndian(&len, sizeof(len));

			fseek(ptr, 2, SEEK_CUR); // Skip to the good stuff

			fread(&segcount, sizeof(segcount), 1, ptr);
			SwapEndian(&segcount, sizeof(segcount));
			segcount >>= 1; // Divide by 2 to get actual seg count

			fseek(ptr, 10, SEEK_CUR); // Skip to the really good stuff
			segendcodes.resize(segcount);
			for (int j = 0; j < segendcodes.size(); j++)
			{
				int16_t end;
				fread(&end, sizeof(end), 1, ptr);
				SwapEndian(&end, sizeof(end));
				segendcodes[j] = end;
			}

			fseek(ptr, 2, SEEK_CUR); // 2-byte pad

			segstartcodes.resize(segcount);
			for (int j = 0; j < segstartcodes.size(); j++)
			{
				int16_t start;
				fread(&start, sizeof(start), 1, ptr);
				SwapEndian(&start, sizeof(start));
				segstartcodes[j] = start;
			}

			segdeltas.resize(segcount);
			for (int j = 0; j < segdeltas.size(); j++)
			{
				int16_t delta;
				fread(&delta, sizeof(delta), 1, ptr);
				SwapEndian(&delta, sizeof(delta));
				segdeltas[j] = delta;
			}

			rangepos = ftell(ptr);
			segrangeoffsets.resize(segcount);
			for (int j = 0; j < segrangeoffsets.size(); j++)
			{
				int16_t offs;
				fread(&offs, sizeof(offs), 1, ptr);
				SwapEndian(&offs, sizeof(offs));
				segrangeoffsets[j] = offs;
			}
			
			for (int j = 0; j < segendcodes.size(); j++)
			{
				for (int k = segstartcodes[j]; k <= segendcodes[j]; k++)
				{
					uint32_t glyfi;
					uint16_t glyf;

					if (segrangeoffsets[j] == 0)
					{
						glyf = segdeltas[j] + k;
						cmap[k] = glyf;
					}
					else
					{
						glyfi = (segrangeoffsets[j] >> 1) + (k - segstartcodes[j]);
						fseek(ptr, rangepos + j * 2 + glyfi * 2, SEEK_SET);
						fread(&glyf, sizeof(glyf), 1, ptr);
						SwapEndian(&glyf, sizeof(glyf));
						cmap[k] = glyf;
					}
				}
			}

			glyfidlen = ftell(ptr) - cmapoffs + 4;
			glyfidlen = len - glyfidlen;
		}
		else if (version == 12)
		{
			struct cmapgroup_t
			{
				uint32_t startcode;
				uint32_t endcode;
				uint32_t startglyf;
			};

			int i;
			int j;

			uint32_t ngroups;
			cmapgroup_t group;

			fseek(ptr, 10, SEEK_CUR);
			fread(&ngroups, sizeof(ngroups), 1, ptr);
			SwapEndian(&ngroups, sizeof(ngroups));

			for (i = 0; i < ngroups; i++)
			{
				fread(&group, sizeof(group), 1, ptr);
				SwapEndian(&group.startcode, sizeof(group.startcode));
				SwapEndian(&group.endcode, sizeof(group.endcode));
				SwapEndian(&group.startglyf, sizeof(group.startglyf));

				for (j = group.startcode; j <= group.endcode; j++)
					cmap[j] = group.startglyf + (j - group.startcode);
			}
		}
		else
		{
			continue;
		}

		return;
	}
}

void TrueTypeFont::LoadHmtx(FILE* ptr)
{
	int i;

	int nmonoglyfs;

	for (i = 0; i < hhdr.nhmetrics; i++)
	{
		fread(&glyfs[i].advw, sizeof(glyfs[i].advw), 1, ptr);
		fread(&glyfs[i].leftbear, sizeof(glyfs[i].leftbear), 1, ptr);

		SwapEndian(&glyfs[i].advw, sizeof(glyfs[i].advw));
		SwapEndian(&glyfs[i].leftbear, sizeof(glyfs[i].leftbear));
	}

	// Take care of the monospace characters at the end the font
	nmonoglyfs = maxp.numglyphs - hhdr.nhmetrics;
	for (i = hhdr.nhmetrics; i < maxp.numglyphs - 1; i++)
	{
		fread(&glyfs[i].leftbear, sizeof(glyfs[i].advw), 1, ptr);
		SwapEndian(&glyfs[i].leftbear, sizeof(glyfs[i].leftbear));
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
	for (i = 0; i < contourends.size(); i++)
	{
		fread(&contourends[i], sizeof(contourends[i]), 1, ptr);
		SwapEndian(&contourends[i], sizeof(contourends[i]));
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
		if (i != 0)
			xcoords[i] = xcoords[i - 1];

		if (flags[i] & 0x02)
		{
			int val;
			ubyte_t b;
			fread(&b, 1, 1, ptr);
			val = b;

			if (~flags[i] & 0x10)
				val = -val;

			xcoords[i] += val;
		}
		else
		{
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
		if (i != 0)
			ycoords[i] = ycoords[i - 1];

		if (flags[i] & 0x04)
		{
			int val;
			ubyte_t b;
			fread(&b, 1, 1, ptr);
			val = b;

			if (~flags[i] & 0x20)
				val = -val;

			ycoords[i] += val;
		}
		else
		{
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
	
	int cstart = 0;
	for (i = 0; i < contourends.size(); i++)
	{
		int npoints = contourends[i] - cstart + 1;
		for (j = cstart; j < cstart + npoints; j++)
		{
			int nextp = cstart + ((j - cstart + 1) % npoints);
			ubyte_t curflags = flags[j];
			ubyte_t nextflags = flags[nextp];

			if ((~curflags & 0x01) && (~nextflags & 0x01)) // Two consecutive control points? Insert a point in between them.
			{
				vec2_t p = Vector2Lerp(points[j], points[nextp], 0.5);
				points.insert(points.begin() + nextp, p);
				flags.insert(flags.begin() + nextp, 0x01); // New point should be on-curve
				for (int k = i; k < contourends.size(); k++)
					contourends[k]++;
				npoints++;
				j++;
			}
		}
		cstart += npoints;
	}

	glyf.points = points;
	glyf.flags = flags;

	cstart = 0;
	for (i = 0; i < contourends.size(); i++)
	{
		int npoints = contourends[i] - cstart + 1;
		for (j = cstart; j < cstart + npoints; j++)
		{
			int nextp = cstart + ((j - cstart + 1) % npoints);
			ubyte_t curflags = flags[j];
			ubyte_t nextflags = flags[nextp];

			if (~curflags & 0x01) // Is the current point a control point? If so, skip
				continue;

			if (nextflags & 0x01)
			{
				line_t line;
				line.p0 = j;
				line.p1 = nextp;

				glyf.lines.push_back(line);
			}
			else
			{
				int next2p = cstart + ((j - cstart + 2) % npoints);
				ubyte_t next2flags = flags[next2p];

				if (next2flags & 0x01)
				{
					bezier_t bezier;
					bezier.p0 = j;
					bezier.p1 = nextp;
					bezier.p2 = next2p;
					glyf.beziers.push_back(bezier);
				}
			}
		}

		cstart += npoints;
	}
	
	glyf.desc = desc;
	glyfs.push_back(glyf);
}

void TrueTypeFont::LoadCompoundGlyph(FILE* ptr, glyphdesc_t desc)
{
	// TODO: Actually implement this
	glyfs.push_back(glyfs[0]);
}

int TrueTypeFont::DrawString(std::string txt, float x, float y, float scale)
{
	int startx = x;
	for (int i = 0; i < txt.size(); i++)
	{
		x += DrawGlyph(txt[i], x, y, scale);
	}

	return x - startx;
}

int TrueTypeFont::DrawCenteredString(std::string txt, float x, float y, float scale)
{
	x -= StringWidth(txt, scale) >> 1;
	return DrawString(txt, x, y, scale);
}

int TrueTypeFont::StringWidth(std::string txt, float scale)
{
	int i;

	int w;

	w = 0;
	for (int i = 0; i < txt.size(); i++)
		w += GlyphWidth(txt[i], scale);

	return w;
}

int TrueTypeFont::DrawGlyph(wchar_t c, float x, float y, float scale)
{
	int i;
	int j;

	int cstart;
	int npoints;
	std::vector<vec2_t> points;

	int g;

	g = IndexCMap(c);

	scale /= hdr.emsize;

	if (c == ' ') // Is the character a space?
	{
		return (glyfs[g].leftbear + glyfs[g].advw) * scale;
	}

	glColor3f(1, 0, 1);

	glBegin(GL_LINES);
	cstart = 0;
	for (i = 0; i < glyfs[g].lines.size(); i++)
	{
		vec2_t p0 = glyfs[g].points[glyfs[g].lines[i].p0] * scale;
		vec2_t p1 = glyfs[g].points[glyfs[g].lines[i].p1] * scale;

		p0.x += x;
		p0.y += y;
		p1.x += x;
		p1.y += y;

		glVertex2f(p0.x, p0.y);
		glVertex2f(p1.x, p1.y);
	}
	glEnd();

	for (i = 0; i < glyfs[g].beziers.size(); i++)
	{
		vec2_t p0 = glyfs[g].points[glyfs[g].beziers[i].p0] * scale;
		vec2_t p1 = glyfs[g].points[glyfs[g].beziers[i].p1] * scale;
		vec2_t p2 = glyfs[g].points[glyfs[g].beziers[i].p2] * scale;

		p0.x += x;
		p0.y += y;
		p1.x += x;
		p1.y += y;
		p2.x += x;
		p2.y += y;

		DrawBezier(p0, p1, p2);
	}

	glPointSize(5.0);
	glBegin(GL_POINTS);
	for (i = 0; i < glyfs[g].points.size(); i++)
	{
		if (glyfs[g].flags[i] & 0x01)
			glColor3f(1, 0, 0);
		else
			glColor3f(0, 0, 1);

		vec2_t p = glyfs[g].points[i] * scale;
		p.x += x;
		p.y += y;
		//glVertex2f(p.x, p.y);
	}
	glEnd();
	glColor3f(1, 1, 1);
	
	return (glyfs[g].advw) * scale;
}

int TrueTypeFont::GlyphWidth(wchar_t c, float scale)
{
	int g;

	g = IndexCMap(c);
	scale /= hdr.emsize;
	return glyfs[g].advw * scale;
}

TrueTypeFont::trimesh_t TrueTypeFont::EarClip(std::vector<vec2_t> contours, std::vector<int> contourends)
{
	std::vector<int> contourstarts(contourends.size());

	trimesh_t mesh;

	std::vector<vec2_t> points(contours.begin(), contours.end());

	// Make a doubly linked list of the points to accelerate vertex removal later
	std::vector<int> next(points.size());
	std::vector<int> last(points.size());

	int cstart = 0;
	for (int c = 0; c < contourends.size(); c++)
	{
		int npoints = contourends[c] - cstart;
		for (int i = cstart; i < contourends[c]; i++)
		{
			next[i] = cstart + ((i - cstart + 1) % npoints);
			last[i] = i - 1;
			while (last[i] < cstart)
				last[i] += npoints;
		}
		cstart += npoints;
	}
	
	int numpoints = contourends[0];
	int valid = contourends[0] - 1;

	int safety = 8192;

	// WARNING TO FUTURE ME: Checking for numpoints > 2 is not a typo, anything lower than two will make an infinite loop. 
	// Don't make the same mistake I did.
	for (int i = valid; numpoints > 2; i = next[i])
	{
		if (safety <= 0) // Most likely in an infinite loop.
		{
			Print::Aegis_Warning("Ear clipping error! This could be a bug in my code, or a bad/too big mesh given.\n");
			break;
		}

		safety--;

		vec2_t p0 = points[last[i]];
		vec2_t p1 = points[i];
		vec2_t p2 = points[next[i]];

		// Calculate winding with "Cross Product" (Cross product in 2d!? What the fuck is going on!?)
		float z = (p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x);

		if (z >= 0) // Ears must be clockwise
			continue;

		bool isear = true;
		for (int j = 0; j < contourends[0]; j++)
		{
			if (j == last[i] || j == i || j == next[i])
				continue;

			if (PointInTriangle(p0, p1, p2, points[j])) // If any points are in the triangle, it's not an ear
			{
				isear = false;
				break;
			}
		}

		if (!isear)
			continue;

		next[last[i]] = next[i];
		last[next[i]] = last[i];
		numpoints--;
		
		mesh.indices.push_back(mesh.points.size());
		mesh.points.push_back(points[last[i]]);
		mesh.indices.push_back(mesh.points.size());
		mesh.points.push_back(points[i]);
		mesh.indices.push_back(mesh.points.size());
		mesh.points.push_back(points[next[i]]);
	}
	
	// OPTIMIZE: Fix this steaming pile of O(n^3) shit
	for (int i = 0; i < mesh.indices.size() - 1; i++) 
	{
		vec2_t v0 = mesh.points[mesh.indices[i]];
		for (int j = i + 1; j < mesh.indices.size(); j++) 
		{
			vec2_t v1 = mesh.points[mesh.indices[j]];
			if (v0.x != v1.x || v0.y != v1.y)
				continue;
			
			int index_to_remove = mesh.indices[j];
			mesh.indices[j] = mesh.indices[i];
			
			mesh.points.erase(mesh.points.begin() + index_to_remove);
			
			for (int k = 0; k < mesh.indices.size(); k++)
			{
				if (mesh.indices[k] > index_to_remove)
					mesh.indices[k]--;
			}

			break;
		}
	}

	return mesh;
}
