#include "SpriteEntity.h"

#include <string>
#include <iostream>

#include "AssetManager.h"

#include "mathutils.h"
#include "binaryloader.h"

SpriteEntity::SpriteEntity(BSPMap& map) : BaseEntity(map)
{

}

SpriteEntity::~SpriteEntity()
{
	//free(mhdr);
}

void SpriteEntity::LoadTexture(char* texture)
{
	std::string filepath = std::string("valve/") + std::string(texture);
	loadBytes(filepath.c_str(), (char**) &mhdr);

	sprpalette_t* palette = (sprpalette_t*)(mhdr + 1);
	int moderncolors[SPRITE_COLORS_IN_PALETTE];
	for (int i = 0; i < SPRITE_COLORS_IN_PALETTE; i++)
	{
		int col = 0;
		switch (mhdr->textureformat)
		{
		case SPRITE_TEXTURE_NORMAL:
			col = 0xFF000000;
			col |= palette->colors[i].r <<  0;
			col |= palette->colors[i].g <<  8;
			col |= palette->colors[i].b << 16;
			break;
		case SPRITE_TEXTURE_ADDITIVE:
			col = 0xFF000000;
			col |= palette->colors[i].r << 0;
			col |= palette->colors[i].g << 8;
			col |= palette->colors[i].b << 16;
			break;
		case SPRITE_TEXTURE_INDEXALPHA:
			col = 0;
			col |= palette->colors[COLORSINPALETTE - 1].r << 0;
			col |= palette->colors[COLORSINPALETTE - 1].g << 8;
			col |= palette->colors[COLORSINPALETTE - 1].b << 16;
			col |= palette->colors[i].r << 24;
			break;
		default:
			break;
		}
		moderncolors[i] = col;
	}

	char* frameoffset = ((char*)mhdr) + sizeof(sprheader_t) + sizeof(sprpalette_t);
	for (int f = 0; f < mhdr->numframes; f++)
	{
		uint32_t* framegroup = (uint32_t*)frameoffset;

		if (*framegroup == 0)
		{
			// texname_f(framenumber)
			std::string texturename = std::string(texture) + std::string("_f") +  std::to_string(f);

			sprframe_t* frame = (sprframe_t*)(frameoffset + sizeof(uint32_t));

			GLuint texname = AssetManager::getInst().getTexture(texturename.c_str(), "spr");
			if (texname != UINT32_MAX)
			{
				texturenames.push_back(texname);
				frames.push_back(frame);
				continue;
			}

			ubyte_t* framedata = (ubyte_t*)(frame + 1);

			std::vector<int> texdata(frame->framewidth * frame->frameheight);
			for (int i = 0; i < frame->framewidth * frame->frameheight; i++)
				texdata[i] = moderncolors[framedata[i]];

			texname = AssetManager::getInst().setTexture(texturename.c_str(), "spr");

			glBindTexture(GL_TEXTURE_2D, texname);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifdef TEXTURE_FILTER
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif // TEXTURE_FILTER

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame->framewidth, frame->frameheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata.data());

			texturenames.push_back(texname);
			frames.push_back(frame);

			frameoffset += sizeof(uint32_t) + sizeof(sprframe_t) + frame->framewidth * frame->frameheight;
		}
		else
		{
			// TODO: In-between frames https://www.gamers.org/dEngine/quake/spec/quake-spec33/qkspec_6.htm#CSPRF
		}
	}
}

void SpriteEntity::Think(float deltatime)
{
	currentframe += deltatime * framerate;

	while (currentframe >= mhdr->numframes)
		currentframe -= mhdr->numframes;
}

void SpriteEntity::Render()
{
	int framenum = (int)currentframe;
	sprframe_t* frame = frames[framenum];

	vec3_t spriteup{};
	vec3_t spriteright{};
	vec3_t spritenormal = NormalizeVector3(camerapos - position);

	switch (mhdr->type)
	{
	case SPRITE_TYPE_PARELLEL:
		spriteright = NormalizeVector3(CrossProduct(cameraup, cameraforward));
		spriteup = CrossProduct(cameraforward, spriteright);
		break;
	case SPRITE_TYPE_PARALLEL_UPRIGHT:
		spriteup = cameraup;
		spriteright = NormalizeVector3(CrossProduct(spritenormal, spriteup));
		break;
	default:
		break;
	}
	
	vec3_t worldpos = position + spriteright * frame->framex + spriteup * frame->framey;

	spriteup = spriteup * frame->frameheight;
	spriteright = spriteright * frame->framewidth;

	vec3_t v[4] =
	{
		{worldpos + spriteright - spriteup},
		{worldpos + spriteright + spriteup},
		{worldpos - spriteright + spriteup},
		{worldpos - spriteright - spriteup},
	};

	vec3_t t[4] =
	{
		{1.0, 1.0},
		{1.0, 0.0},
		{0.0, 0.0},
		{0.0, 1.0},
	};
	
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texturenames[framenum]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glCullFace(GL_FRONT);

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++)
	{
		glTexCoord2f(t[i].x, t[i].y);
		glVertex3f(v[i].x, v[i].y, v[i].z);
	}
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}