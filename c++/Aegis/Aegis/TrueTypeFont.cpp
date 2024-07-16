#include "TrueTypeFont.h"

#include <GL/glew.h>

#include <string>

#include "Print.h"

#include "mathutils.h"

#include "binaryloader.h"

#include "Game.h"

bool TrueTypeFont::Load(std::string name)
{
	int i;
	FILE* ptr;
	offsetsubtable_t offsettable;

	uint16_t glyfoffs;

	ptr = fopen(name.c_str(), "rb");
	if (ptr == nullptr)
		return false;

	fread(&offsettable, sizeof(offsettable), 1, ptr);
	BigEndian(&offsettable.scalartype, sizeof(offsettable.scalartype));
	BigEndian(&offsettable.numtables, sizeof(offsettable.numtables));
	BigEndian(&offsettable.searchrange, sizeof(offsettable.searchrange));
	BigEndian(&offsettable.entryselector, sizeof(offsettable.entryselector));
	BigEndian(&offsettable.rangeshift, sizeof(offsettable.rangeshift));

	tagdirs.clear();
	for (i = 0; i < offsettable.numtables; i++)
	{
		tabledir_t dir;
		fread(&dir, sizeof(dir), 1, ptr);
		BigEndian(&dir.checksum, sizeof(dir.checksum));
		BigEndian(&dir.offset, sizeof(dir.offset));
		BigEndian(&dir.length, sizeof(dir.length));

		std::string tag(4, 0);
		tag[0] = dir.tag[0];
		tag[1] = dir.tag[1];
		tag[2] = dir.tag[2];
		tag[3] = dir.tag[3];
		tagdirs[tag] = dir;
	}

	fseek(ptr, tagdirs["cmap"].offset, SEEK_SET);
	LoadCMap(ptr);
	
	int locaoffsetsize;
	fseek(ptr, tagdirs["head"].offset, SEEK_SET);
	fread(&hdr, sizeof(hdr), 1, ptr);
	BigEndian(&hdr.version, sizeof(hdr.version));
	BigEndian(&hdr.revision, sizeof(hdr.revision));
	BigEndian(&hdr.checksumadj, sizeof(hdr.checksumadj));
	BigEndian(&hdr.magic, sizeof(hdr.magic));
	BigEndian(&hdr.flags, sizeof(hdr.flags));
	BigEndian(&hdr.emsize, sizeof(hdr.emsize));
	BigEndian(&hdr.createddate, sizeof(hdr.createddate));
	BigEndian(&hdr.modifieddate, sizeof(hdr.modifieddate));
	BigEndian(&hdr.xmin, sizeof(hdr.xmin));
	BigEndian(&hdr.ymin, sizeof(hdr.ymin));
	BigEndian(&hdr.xmax, sizeof(hdr.xmax));
	BigEndian(&hdr.ymax, sizeof(hdr.ymax));
	BigEndian(&hdr.macstyle, sizeof(hdr.macstyle));
	BigEndian(&hdr.smallestsize, sizeof(hdr.smallestsize));
	BigEndian(&hdr.directionhint, sizeof(hdr.directionhint));
	BigEndian(&hdr.locaformat, sizeof(hdr.locaformat));
	BigEndian(&hdr.glyphformat, sizeof(hdr.glyphformat));
	locaoffsetsize = hdr.locaformat ? 4 : 2;

	fseek(ptr, tagdirs["maxp"].offset, SEEK_SET);
	fread(&maxp, sizeof(maxp), 1, ptr);
	BigEndian(&maxp.version, sizeof(maxp.version));
	BigEndian(&maxp.numglyphs, sizeof(maxp.numglyphs));
	BigEndian(&maxp.maxpoints, sizeof(maxp.maxpoints));
	BigEndian(&maxp.maxcontours, sizeof(maxp.maxcontours));
	BigEndian(&maxp.maxcomponentpoints, sizeof(maxp.maxcomponentpoints));
	BigEndian(&maxp.maxcomponentcontours, sizeof(maxp.maxcomponentcontours));
	BigEndian(&maxp.maxzones, sizeof(maxp.maxzones));
	BigEndian(&maxp.maxtwilightpoints, sizeof(maxp.maxtwilightpoints));
	BigEndian(&maxp.maxstorage, sizeof(maxp.maxstorage));
	BigEndian(&maxp.maxfunctiondefs, sizeof(maxp.maxfunctiondefs));
	BigEndian(&maxp.maxinstructiondefs, sizeof(maxp.maxinstructiondefs));
	BigEndian(&maxp.maxstackelements, sizeof(maxp.maxstackelements));
	BigEndian(&maxp.maxinstructionsize, sizeof(maxp.maxinstructionsize));
	BigEndian(&maxp.maxcomponentel, sizeof(maxp.maxcomponentel));
	BigEndian(&maxp.maxcomponentdepth, sizeof(maxp.maxcomponentdepth));

	fseek(ptr, tagdirs["hhea"].offset, SEEK_SET);
	fread(&hhdr, sizeof(hhdr), 1, ptr);
	BigEndian(&hhdr.version, sizeof(hhdr.version));
	BigEndian(&hhdr.ascent, sizeof(hhdr.ascent));
	BigEndian(&hhdr.descent, sizeof(hhdr.descent));
	BigEndian(&hhdr.linegap, sizeof(hhdr.linegap));
	BigEndian(&hhdr.maxadvw, sizeof(hhdr.maxadvw));
	BigEndian(&hhdr.minleftbear, sizeof(hhdr.minleftbear));
	BigEndian(&hhdr.minrightbear, sizeof(hhdr.minrightbear));
	BigEndian(&hhdr.maxxextend, sizeof(hhdr.maxxextend));
	BigEndian(&hhdr.caretrise, sizeof(hhdr.caretrise));
	BigEndian(&hhdr.caretrun, sizeof(hhdr.caretrun));
	BigEndian(&hhdr.caretoffset, sizeof(hhdr.caretoffset));
	BigEndian(&hhdr.datafmt, sizeof(hhdr.datafmt));
	BigEndian(&hhdr.nhmetrics, sizeof(hhdr.nhmetrics));

	fontheight = hhdr.ascent - hhdr.descent + hhdr.linegap;

	localengths.resize(maxp.numglyphs + 1);
	locaoffsets.resize(maxp.numglyphs + 1);

	fseek(ptr, tagdirs["loca"].offset, SEEK_SET);
	for (i = 0; i < maxp.numglyphs + 1; i++)
	{
		uint32_t offset;
		if (locaoffsetsize == 2)
		{
			uint16_t offs;
			fread(&offs, sizeof(offs), 1, ptr);
			BigEndian(&offs, sizeof(offs));
			offset = offs;
		}
		else
		{
			uint32_t offs;
			fread(&offs, sizeof(offs), 1, ptr);
			BigEndian(&offs, sizeof(offs));
			offset = offs;
		}

		locaoffsets[i] = offset;
	}

	for (i = 0; i < maxp.numglyphs; i++)
	{
		fseek(ptr, tagdirs["glyf"].offset + (locaoffsets[i] << 1), SEEK_SET);
		glyfs.push_back(LoadGlyph(ptr));
	}

	fseek(ptr, tagdirs["hmtx"].offset, SEEK_SET);
	LoadHmtx(ptr);

	fclose(ptr);
	return true;
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
	BigEndian(&hdr.version, sizeof(hdr.version));
	BigEndian(&hdr.nsubtables, sizeof(hdr.nsubtables));

	subtables.resize(hdr.nsubtables);
	for (i = 0; i < hdr.nsubtables; i++)
	{
		fread(&subtables[i], sizeof(subtables[i]), 1, ptr);
		BigEndian(&subtables[i].platid, sizeof(subtables[i].platid));
		BigEndian(&subtables[i].platspec, sizeof(subtables[i].platspec));
		BigEndian(&subtables[i].offset, sizeof(subtables[i].offset));
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
		BigEndian(&version, sizeof(version));
		cmapformat = version;
		platformspec = subtables[i].platspec;

		if (version == 4)
		{
			uint16_t len;
			uint16_t glyfidlen = 0;
			uint16_t segcount;

			int rangepos;

			fread(&len, sizeof(len), 1, ptr);
			BigEndian(&len, sizeof(len));

			fseek(ptr, 2, SEEK_CUR); // Skip to the good stuff

			fread(&segcount, sizeof(segcount), 1, ptr);
			BigEndian(&segcount, sizeof(segcount));
			segcount >>= 1; // Divide by 2 to get actual seg count

			fseek(ptr, 6, SEEK_CUR); // Skip to the really good stuff
			segendcodes.resize(segcount);
			for (int j = 0; j < segendcodes.size(); j++)
			{
				uint16_t end;
				fread(&end, sizeof(end), 1, ptr);
				BigEndian(&end, sizeof(end));
				segendcodes[j] = end;
			}

			fseek(ptr, 2, SEEK_CUR); // 2-byte pad

			segstartcodes.resize(segcount);
			for (int j = 0; j < segstartcodes.size(); j++)
			{
				uint16_t start;
				fread(&start, sizeof(start), 1, ptr);
				BigEndian(&start, sizeof(start));
				segstartcodes[j] = start;
			}

			segdeltas.resize(segcount);
			for (int j = 0; j < segdeltas.size(); j++)
			{
				uint16_t delta;
				fread(&delta, sizeof(delta), 1, ptr);
				BigEndian(&delta, sizeof(delta));
				segdeltas[j] = delta;
			}

			rangepos = ftell(ptr);
			segrangeoffsets.resize(segcount);
			for (int j = 0; j < segrangeoffsets.size(); j++)
			{
				uint16_t offs;
				fread(&offs, sizeof(offs), 1, ptr);
				BigEndian(&offs, sizeof(offs));
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
					}
					else
					{
						glyfi = segrangeoffsets[j] + ((k - segstartcodes[j]) << 1);
						fseek(ptr, rangepos + (j << 1) + glyfi, SEEK_SET);
						fread(&glyf, sizeof(glyf), 1, ptr);
						BigEndian(&glyf, sizeof(glyf));
					}
					cmap[k] = glyf;
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
			BigEndian(&ngroups, sizeof(ngroups));

			for (i = 0; i < ngroups; i++)
			{
				fread(&group, sizeof(group), 1, ptr);
				BigEndian(&group.startcode, sizeof(group.startcode));
				BigEndian(&group.endcode, sizeof(group.endcode));
				BigEndian(&group.startglyf, sizeof(group.startglyf));

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

	if (hhdr.nhmetrics > glyfs.size())
		hhdr.nhmetrics = glyfs.size();

	for (i = 0; i < hhdr.nhmetrics; i++)
	{
		fread(&glyfs[i].advw, sizeof(glyfs[i].advw), 1, ptr);
		fread(&glyfs[i].leftbear, sizeof(glyfs[i].leftbear), 1, ptr);

		BigEndian(&glyfs[i].advw, sizeof(glyfs[i].advw));
		BigEndian(&glyfs[i].leftbear, sizeof(glyfs[i].leftbear));
	}

	// Take care of the monospace characters at the end the font
	nmonoglyfs = maxp.numglyphs - hhdr.nhmetrics;
	for (i = hhdr.nhmetrics; i < maxp.numglyphs - 1; i++)
	{
		fread(&glyfs[i].leftbear, sizeof(glyfs[i].advw), 1, ptr);
		BigEndian(&glyfs[i].leftbear, sizeof(glyfs[i].leftbear));
	}
}

TrueTypeFont::glyph_t TrueTypeFont::LoadGlyph(FILE * ptr)
{
	glyphdesc_t glyfdesc;

	fread(&glyfdesc, sizeof(glyfdesc), 1, ptr);
	BigEndian(&glyfdesc.ncontours, sizeof(glyfdesc.ncontours));
	BigEndian(&glyfdesc.xmin, sizeof(glyfdesc.xmin));
	BigEndian(&glyfdesc.ymin, sizeof(glyfdesc.ymin));
	BigEndian(&glyfdesc.xmax, sizeof(glyfdesc.xmax));
	BigEndian(&glyfdesc.ymax, sizeof(glyfdesc.ymax));

	if (glyfdesc.ncontours < 0)
		return LoadCompoundGlyph(ptr, glyfdesc);
	else
		return LoadSimpleGlyph(ptr, glyfdesc);
}

TrueTypeFont::glyph_t TrueTypeFont::LoadSimpleGlyph(FILE* ptr, glyphdesc_t desc)
{
	int i;
	int j;
	int k;

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
		BigEndian(&contourends[i], sizeof(contourends[i]));
	}

	fread(&instructionlength, sizeof(instructionlength), 1, ptr);
	BigEndian(&instructionlength, sizeof(instructionlength));
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
				BigEndian(&val, sizeof(val));

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
				BigEndian(&val, sizeof(val));

				ycoords[i] += val;
			}
		}

		points[i].y = ycoords[i];
	}
	
	std::vector<vec2_t> meshpoints = points;
	std::vector<uint16_t> meshends = contourends;
	std::vector<ubyte_t> meshflags = flags;

	int cstart = 0;
	for (i = 0; i < contourends.size(); i++)
	{
		int npoints = contourends[i] - cstart + 1;
		std::vector<vec2_t> cur(points.begin() + cstart, points.begin() + cstart + npoints);
		float dir = PolygonDirection(cur);
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

	cstart = 0;
	for (i = 0; i < meshends.size(); i++)
	{
		int npoints = meshends[i] - cstart + 1;
		std::vector<vec2_t> cur(meshpoints.begin() + cstart, meshpoints.begin() + cstart + npoints);
		float dir = PolygonDirection(cur);

		if (dir < 0.0)
		{
			for (j = cstart; j < cstart + npoints; j++)
			{
				int nextp = cstart + ((j - cstart + 1) % npoints);
				ubyte_t curflags = meshflags[j];
				ubyte_t nextflags = meshflags[nextp];

				if ((~curflags & 0x01) && (~nextflags & 0x01)) // Two consecutive control points? Insert a point in between them.
				{
					vec2_t p = Vector2Lerp(meshpoints[j], meshpoints[nextp], 0.5);
					meshpoints.insert(meshpoints.begin() + nextp, p);
					meshflags.insert(meshflags.begin() + nextp, 0x01); // New point should be on-curve
					for (int k = i; k < meshends.size(); k++)
						meshends[k]++;
					npoints++;
					j++;
				}
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
	
	std::vector<int> markpoints;
	cstart = 0;
	for (i = 0; i < meshends.size(); i++)
	{
		int npoints = meshends[i] - cstart + 1;
		std::vector<vec2_t> cur(meshpoints.begin() + cstart, meshpoints.begin() + meshends[i] + 1);
		for (j = cstart; j < cstart + npoints; j++)
		{
			int last = cstart + ((j - cstart - 1 + npoints) % npoints);
			int next = cstart + ((j - cstart + 1) % npoints);
			vec2_t v0 = meshpoints[last];
			vec2_t v1 = meshpoints[j];
			vec2_t v2 = meshpoints[next];
			bool dir = TriangleClockwise2D(v0, v1, v2);
			if(dir) // Clockwise; Contour
			{
				if (~meshflags[j] & 1)
					markpoints.push_back(j);
			}
		}

		cstart += npoints;
	}

	std::sort(markpoints.begin(), markpoints.end());
	markpoints.resize(std::distance(markpoints.begin(), std::unique(markpoints.begin(), markpoints.end())));
	std::reverse(markpoints.begin(), markpoints.end());
	for (i = 0; i < markpoints.size(); i++)
	{
		meshpoints.erase(meshpoints.begin() + markpoints[i]);
		for (j = 0; j < meshends.size(); j++)
		{
			if (meshends[j] >= markpoints[i])
				meshends[j]--;
		}
	}

	cstart = 0;
	for (i = 0; i < contourends.size(); i++)
	{
		int npoints = contourends[i] - cstart + 1;
		for (j = cstart; j < cstart + npoints; j++)
		{
			if (flags[j] & 0x01) // On-curve
				continue;

			std::vector<vec2_t> fan;

			vec2_t p0 = glyf.points[cstart + ((j - cstart - 1 + npoints) % npoints)];
			vec2_t p1 = glyf.points[j];
			vec2_t p2 = glyf.points[cstart + ((j - cstart + 1) % npoints)];

			bool dir = TriangleClockwise2D(p0, p1, p2);
			if (dir)
				fan.push_back(Vector2Lerp(p0, p2, 0.5));
			else
				fan.push_back(p1);
			
			const int segs = 16;
			float t = 0.0;
			float step = 1.0 / (float)segs;
			for (k = 0; k < segs + 1; k++, t += step)
				fan.push_back(Vector2Bezier(p0, p1, p2, t));

			glyf.bezierfans.push_back(fan);
		}
		cstart += npoints;
	}

	glyf.triangles = EarClip(meshpoints, meshends);

	glyf.ends = contourends;

	return glyf;
}

TrueTypeFont::glyph_t TrueTypeFont::LoadCompoundGlyph(FILE* ptr, glyphdesc_t desc)
{
	int i;
	int j;

	glyph_t glyf;
	int npoints;
	
	uint32_t before;
	glyph_t curglyf;
	vec2_t p;

	uint16_t flags;
	uint16_t glyfi;
	byte_t temp8;
	int16_t temp16;
	uint16_t tempu16;
	float a, b, c, d, e, f, m, n;

	npoints = 0;
	before = ftell(ptr);
	flags = 0x0020;
	while (flags & 0x0020)
	{
		fseek(ptr, before, SEEK_SET);

		fread(&flags, sizeof(flags), 1, ptr);
		BigEndian(&flags, sizeof(flags));
		fread(&glyfi, sizeof(glyfi), 1, ptr);
		BigEndian(&glyfi, sizeof(glyfi));

		if (~flags & 0x0002)
		{
			Print::Aegis_Warning("Unsupported compound glyph argument types.\n");
			break;
		}

		a = b = c = d = e = f = 0.0;

		switch (flags & 0x0001)
		{
		case 0:
			fread(&temp8, sizeof(temp8), 1, ptr);
			e = temp8;
			fread(&temp8, sizeof(temp8), 1, ptr);
			f = temp8;
			break;
		case 1:
			fread(&temp16, sizeof(temp16), 1, ptr);
			BigEndian(&temp16, sizeof(temp16));
			e = temp16;
			fread(&temp16, sizeof(temp16), 1, ptr);
			BigEndian(&temp16, sizeof(temp16));
			f = temp16;
			break;
		default:
			break;
		}

		if (flags & 0x0008)
		{
			fread(&temp16, sizeof(temp16), 1, ptr);
			BigEndian(&temp16, sizeof(temp16));
			a = d = (float)temp16 / (float)(1 << 13); // Convert fixed 2.14 to float
		}
		else if (flags & 0x0040)
		{
			fread(&temp16, sizeof(temp16), 1, ptr);
			BigEndian(&temp16, sizeof(temp16));
			a = (float)temp16 / (float)(1 << 13); // Convert fixed 2.14 to float

			fread(&temp16, sizeof(temp16), 1, ptr);
			BigEndian(&temp16, sizeof(temp16));
			d = (float)temp16 / (float)(1 << 13); // Convert fixed 2.14 to float
		}
		else if (flags & 0x0080)
		{
			fread(&temp16, sizeof(temp16), 1, ptr);
			BigEndian(&temp16, sizeof(temp16));
			a = (float)temp16 / (float)(1 << 13); // Convert fixed 2.14 to float

			fread(&temp16, sizeof(temp16), 1, ptr);
			BigEndian(&temp16, sizeof(temp16));
			b = (float)temp16 / (float)(1 << 13); // Convert fixed 2.14 to float

			fread(&temp16, sizeof(temp16), 1, ptr);
			BigEndian(&temp16, sizeof(temp16));
			c = (float)temp16 / (float)(1 << 13); // Convert fixed 2.14 to float

			fread(&temp16, sizeof(temp16), 1, ptr);
			BigEndian(&temp16, sizeof(temp16));
			d = (float)temp16 / (float)(1 << 13); // Convert fixed 2.14 to float
		}
		else
		{
			a = 1.0;
			d = 1.0;
		}

		m = maxf(fabsf(a), fabsf(b));
		n = maxf(fabsf(c), fabsf(d));

		if (fabsf(a) - fabsf(c) < (33.0f / 65536.0f))
			m *= 2.0;

		if (fabsf(b) - fabsf(d) < (33.0f / 65536.0f))
			n *= 2.0;

		before = ftell(ptr);

		fseek(ptr, tagdirs["glyf"].offset + (locaoffsets[glyfi] << 1), SEEK_SET);
		if (glyfi < glyfs.size())
			curglyf = glyfs[glyfi];
		else
			curglyf = LoadGlyph(ptr);

		for (j = 0; j < curglyf.points.size(); j++)
		{
			p.x = a * curglyf.points[j].x + c * curglyf.points[j].y + m * e;
			p.y = b * curglyf.points[j].x + d * curglyf.points[j].y + n * f;

			glyf.points.push_back(p);
		}

		for (j = 0; j < curglyf.ends.size(); j++)
			glyf.ends.push_back(curglyf.ends[j] + npoints);

		npoints += curglyf.points.size();
	}

	glyf.triangles = EarClip(glyf.points, glyf.ends);
	return glyf;
}

int TrueTypeFont::DrawString(std::string txt, float x, float y, float scale)
{
	int i;
	int j;

	char c;

	std::vector<std::string> split;
	std::string cur;

	int originx = x;

	for (i = 0; i < txt.size(); i++)
	{
		c = txt[i];
		if (c == '\n')
		{
			if (!cur.empty())
			{
				split.push_back(cur);
				cur.clear();
			}
		}
		else
			cur += c;
	}

	if (!cur.empty())
		split.push_back(cur);

	int biggestx = 0;
	for (i = 0; i < split.size(); i++, y -= fontheight)
	{
		cur = split[i];
		
		int w = DrawLine(cur, x, y, scale, false);

		if (w > biggestx)
			biggestx = w;
	}

	return biggestx;
}

int TrueTypeFont::DrawCenteredString(std::string txt, float x, float y, float scale)
{
	int i;
	int j;

	char c;

	std::vector<std::string> split;
	std::string cur;

	int originx = x;

	for (i = 0; i < txt.size(); i++)
	{
		c = txt[i];
		if (c == '\n')
		{
			if (!cur.empty())
			{
				split.push_back(cur);
				cur.clear();
			}
		}
		else
			cur += c;
	}

	if (!cur.empty())
		split.push_back(cur);

	int biggestx = 0;
	for (i = 0; i < split.size(); i++, y -= (fontheight * (scale / hdr.emsize)))
	{
		cur = split[i];

		int w = DrawLine(cur, x, y, scale, true);

		if (w > biggestx)
			biggestx = w;
	}

	return biggestx;
}

int TrueTypeFont::DrawLine(std::string txt, float x, float y, float scale, bool center)
{
	if(center)
		x -= StringWidth(txt, scale) >> 1;

	int startx = x;
	for (int i = 0; i < txt.size(); i++)
		x += DrawGlyph(txt[i], x, y, scale);

	return x - startx;
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

	if (glyfs.size() == 0)
		return 0;

	int g;

	g = IndexCMap(c);

	scale /= hdr.emsize;

	if (c == ' ') // Is the character a space?
		return (glyfs[g].leftbear + glyfs[g].advw) * scale;
	
	glEnable(GL_MULTISAMPLE);
	
	glBegin(GL_TRIANGLES);
	for (i = 0; i < glyfs[g].triangles.indices.size(); i += 3)
	{
		vec2_t p0 = glyfs[g].triangles.points[glyfs[g].triangles.indices[i + 0]] * scale;
		vec2_t p1 = glyfs[g].triangles.points[glyfs[g].triangles.indices[i + 1]] * scale;
		vec2_t p2 = glyfs[g].triangles.points[glyfs[g].triangles.indices[i + 2]] * scale;

		p0.x += x;
		p0.y += y;
		p1.x += x;
		p1.y += y;
		p2.x += x;
		p2.y += y;

		glVertex2f(p0.x, p0.y);
		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
	}
	glEnd();

	for (i = 0; i < glyfs[g].bezierfans.size(); i++)
	{
		glBegin(GL_TRIANGLE_FAN);
		for (j = 0; j < glyfs[g].bezierfans[i].size(); j++)
		{
			vec2_t p = glyfs[g].bezierfans[i][j] * scale;

			p.x += x;
			p.y += y;

			glVertex2f(p.x, p.y);
		}
		glEnd();
	}
	
	glDisable(GL_MULTISAMPLE);

	return (glyfs[g].advw) * scale;
}

int TrueTypeFont::GlyphWidth(wchar_t c, float scale)
{
	int g;

	g = IndexCMap(c);
	scale /= hdr.emsize;
	return glyfs[g].advw * scale;
}

TrueTypeFont::trimesh_t TrueTypeFont::EarClip(std::vector<vec2_t> points, std::vector<uint16_t> contourends)
{
	trimesh_t mesh;

	std::vector<std::vector<vec2_t>> contours;
	std::vector<std::vector<vec2_t>> holes;

	int cstart = 0;
	for (int c = 0; c < contourends.size(); c++)
	{
		std::vector<vec2_t> contour = std::vector<vec2_t>(points.begin() + cstart, points.begin() + contourends[c] + 1);
		float dir = PolygonDirection(contour);
		if (dir > 0)
			holes.push_back(contour); // Counter clockwise contours are holes
		else
			contours.push_back(contour); // Clockwise contours are solid

		cstart = contourends[c] + 1;
	}

	// Cut into contours with holes
	for (int c = 0; c < contours.size(); c++)
	{
		std::vector<vec2_t> contour = contours[c];
		for (int h = 0; h < holes.size(); h++)
		{
			if (!PointInPolygon2D(contour, holes[h][0])) // Only use holes in the contour
				continue;

			vec2_t p = holes[h][0];
			int pi = 0; // Point index, not π!
			for (int i = 0; i < holes[h].size(); i++)
			{
				if (holes[h][i].x > p.x)
				{
					p = holes[h][i];
					pi = i;
				}
			}

			// Find a good point to "bridge" the contour and its hole.
			// OPTIMIZE: Compute the triangle intersection once at the end instead of every time
			int closestp = -1;
			float closestdist = 99999999.0;
			for (int i = 0; i < contour.size(); i++)
			{
				vec2_t e0 = contour[i] - p;
				vec2_t e1 = contour[(i + 1) % contour.size()] - p;

				float dst;
				if (!SegXIntercept(e0, e1, &dst))
					continue;

				if (dst < closestdist)
				{
					vec2_t p0 = p;
					p0.x += dst;
					vec2_t p1 = e0 + p;
					if (e1.x > p1.x)
						p1 = e1 + p;

					vec2_t p2 = p;

					dst = (p1.x - p.x) * (p1.x - p.x) + (p1.y - p.y) * (p1.y - p.y);
					int closestcand = contour[i].x > contour[(i + 1) % contour.size()].x ? i : (i + 1) % contour.size();
					for (int j = 0; j < contour.size(); j++)
					{
						vec2_t cand = contour[j];
						if (!PointInTriangle(p0, p1, p2, cand))
							continue;

						float d = (cand.x - p.x) * (cand.x - p.x) + (cand.y - p.y) * (cand.y - p.y);
						if (d < dst)
						{
							closestcand = j;
							dst = d;
						}
					}

					if (closestcand == -1)
						continue;

					closestdist = dst;
					closestp = closestcand;
				}
			}

			if (closestp < 0)
			{
				// How did we get here?
				Print::Aegis_Warning("Ear clipping function error.\n");
				return {};
			}
			
			int insertindex = closestp;
			for (int i = pi; i < pi + holes[h].size() + 1; i++)
			{
				vec2_t v = holes[h][i % holes[h].size()];
				contour.insert(contour.begin() + insertindex + 1, v);
				insertindex++;
			}
			contour.insert(contour.begin() + insertindex + 1, contour[closestp]);
		}
		contours[c] = contour;
	}

	for (int c = 0; c < contours.size(); c++)
	{
		if (contours[c].size() < 1)
			continue;

		contour = contours[c];

		// Make a doubly linked list of the points to accelerate vertex removal later
		next.resize(contour.size());
		last.resize(contour.size());
		convex.resize(contour.size());
		int numpoints = contour.size();
		for (int i = 0; i < contour.size(); i++)
		{
			next[i] = (i + 1) % numpoints;
			last[i] = (i - 1 + numpoints) % numpoints;

			vec2_t v0 = contour[last[i]];
			vec2_t v1 = contour[i];
			vec2_t v2 = contour[next[i]];
			convex[i] = TriangleClockwise2D(v0, v1, v2);
		}

		valid = last[0]; // Keep track of any valid vertex still in the polygon
		while (numpoints > 2)
		{
			bool found = false;

			int nloops;
			int loopcheck;
			for (int i = valid, nloops = 0, loopcheck = valid; nloops < 2; i = last[i])
			{
				if (i == valid)
					nloops++;

				if (!convex[i])
					continue;

				if (VertInTri(i))
					continue;
				
				mesh.points.push_back(contour[last[i]]);
				mesh.indices.push_back(mesh.points.size() - 1);
				
				mesh.points.push_back(contour[i]);
				mesh.indices.push_back(mesh.points.size() - 1);

				mesh.points.push_back(contour[next[i]]);
				mesh.indices.push_back(mesh.points.size() - 1);

				next[last[i]] = next[i];
				last[next[i]] = last[i];

				if (!convex[last[i]])
					convex[last[i]] = TriangleClockwise2D(contour[last[last[i]]], contour[last[i]], contour[next[last[i]]]);

				if (!convex[next[i]])
					convex[next[i]] = TriangleClockwise2D(contour[last[next[i]]], contour[next[i]], contour[next[next[i]]]);

				if (i == valid)
					valid = last[i];
				
				numpoints--;
				found = true;
			}

			if (!found)
			{
				Print::Aegis_Warning("Ear Clipping failed: bad mesh given or bug.\n");
				return mesh;
			}
		}
	}
	
		if (mesh.indices.size() < 1)
			return {};

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

		// Flip any clockwise triangles
		for (int i = 0; i < mesh.indices.size(); i += 3)
		{
			vec2_t v0 = mesh.points[mesh.indices[i + 0]];
			vec2_t v1 = mesh.points[mesh.indices[i + 1]];
			vec2_t v2 = mesh.points[mesh.indices[i + 2]];

			// Skip if its already CC
			// NOTE: For some reason we're skipping if its clockwise. It works for some reason.
			if (TriangleClockwise2D(v0, v1, v2))
				continue;

			int temp = mesh.indices[i];
			mesh.indices[i] = mesh.indices[i + 2];
			mesh.indices[i + 2] = temp;
		}

		return mesh;
	}

bool TrueTypeFont::VertInTri(int v)
{
	vec2_t v0 = contour[last[v]];
	vec2_t v1 = contour[v];
	vec2_t v2 = contour[next[v]];

	int nloops;
	for (int i = valid, nloops = 0; nloops < 2; i = next[i])
	{
		if (i == valid)
			nloops++;

		if (i == last[v] || i == v || i == next[v])
			continue;

		if (convex[i])
			continue;

		if (PointInTriangle(v0, v1, v2, contour[i]))
			return true;
	}

	return false;
}
