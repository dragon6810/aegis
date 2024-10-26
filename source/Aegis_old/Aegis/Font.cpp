#include "Font.h"

#include <vector>

#include "binaryloader.h"

#include "defs.h"

#include "AssetManager.h"

void Font::Load(std::string font, std::string wad)
{
	wadheader_t* mhdr;
	loadBytes(wad.c_str(), (char**)&mhdr);
	
	waddirentry_t* entry = nullptr;
	for (int i = 0; i < mhdr->numdirs; i++)
	{
		waddirentry_t* curentry = (waddirentry_t*)((char*)mhdr + mhdr->diroffset) + i;

		if (strcmp(font.c_str(), curentry->name) == 0)
		{
			entry = curentry;
			break;
		}
	}

	if (entry == nullptr)
	{
		printf("*WARNING* Couldn't find font \"%s\" in wad \"%s\"!\n", font.c_str(), wad.c_str());
		memset(glyphs, 0, sizeof(glyph_t) * FONT_NGLYPHS);
		return;
	}
	
	wadfont_t* fontentry = (wadfont_t*)((char*)mhdr + entry->entryoffset);
	wadcharinfo_t* charinfo = (wadcharinfo_t*)(fontentry + 1);

	atlaswidth = fontentry->width * FONT_ROWGLYPHS;
	atlasheight = fontentry->height;

	for (int i = 0; i < FONT_NGLYPHS; i++)
	{
		glyphs[i].width = charinfo[i].charwidth;
		glyphs[i].height = fontentry->rowheight;
		glyphs[i].xoffset = charinfo[i].startoffset % atlaswidth;
		glyphs[i].yoffset = charinfo[i].startoffset / atlaswidth;
	}

	color24_t* palette = (color24_t*)((char*)(charinfo + FONT_NGLYPHS) + fontentry->width * fontentry->height + sizeof(short));
	int modernpalette[COLORSINPALETTE];
	for (int i = 0; i < COLORSINPALETTE; i++)
	{
		color24_t col = palette[i];

		modernpalette[i] = 0xFF000000;
		if (col.r == 255 && col.g == 255 && col.b == 255)
		{
			modernpalette[i] = 0;
			continue;
		}

		modernpalette[i] |= col.g <<  0;
		modernpalette[i] |= col.g <<  8;
		modernpalette[i] |= col.g << 16;
	}

	std::vector<int> texdata(atlaswidth * atlasheight);
	for (int i = 0; i < atlaswidth * atlasheight; i++)
	{
		ubyte_t value = ((ubyte_t*)(fontentry + 1) + sizeof(wadcharinfo_t) * FONT_NGLYPHS)[i];
		texdata[i] = modernpalette[value];
	}

	fonttex = AssetManager::getInst().setTexture(font.c_str(), wad.c_str());

	glBindTexture(GL_TEXTURE_2D, fonttex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlaswidth, atlasheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata.data());

	free(mhdr);
}

int Font::GetStringWidth(std::string text)
{
	int width = 0;
	for (int i = 0; i < text.size(); i++)
	{
		ubyte_t c = text[i];
		width += glyphs[c].width;
	}

	return width;
}

int Font::GetHeight()
{
	return glyphs[0].height;
}

int Font::DrawGlyph(char c, int x, int y)
{
	glyph_t glyph = glyphs[(ubyte_t) c];
	vec2_t uv = { (float)glyph.xoffset / (float)atlaswidth, (float)glyph.yoffset / (float)atlasheight };
	vec2_t dims = { (float)glyph.width / (float)atlaswidth, (float)glyph.height / (float)atlasheight };
	
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, fonttex);
	glBegin(GL_QUADS);

	glTexCoord2f(uv.x, uv.y);
	glVertex2f(x, y + glyph.height);
	glTexCoord2f(uv.x + dims.x, uv.y);
	glVertex2f(x + glyph.width, y + glyph.height);
	glTexCoord2f(uv.x + dims.x, uv.y + dims.y);
	glVertex2f(x + glyph.width, y);
	glTexCoord2f(uv.x, uv.y + dims.y);
	glVertex2f(x, y);

	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	return glyph.width;
}

int Font::DrawString(std::string text, int x, int y)
{
	int originalx = x;
	for (int i = 0; i < text.size(); i++)
	{
		x += DrawGlyph(text[i], x, y);
	}

	return x - originalx;
}