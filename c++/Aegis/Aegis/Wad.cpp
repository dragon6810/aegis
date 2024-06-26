#include "Wad.h"

#include <GL/glew.h>

#include <stdlib.h>

#include "binaryloader.h"
#include "loadtexture.h"
#include "AssetManager.h"

void Wad::Load(const char* filename)
{
	loadBytes(filename, (char**)&whdr);

	for (int i = 0; i < whdr->numdirs; i++)
	{
		waddirentry_t* entry = (waddirentry_t*)((char*)whdr + whdr->diroffset) + i;

		GLuint texindex = AssetManager::getInst().setTexture(entry->name, "wad");

		int** texdata = (int**)malloc(sizeof(int*) * BSP_MIPLEVELS);
		int width, height;

		miptex_t* miptex = (miptex_t*)((char*)whdr + entry->entryoffset);

		loadmiptex((char*)miptex, texdata, &width, &height);
		
		glBindTexture(GL_TEXTURE_2D, texindex);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifdef TEXTURE_FILTER
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif // TEXTURE_FILTER

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, BSP_MIPLEVELS - 1);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width >> 0, height >> 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[0]);
		glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA, width >> 1, height >> 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[1]);
		glTexImage2D(GL_TEXTURE_2D, 2, GL_RGBA, width >> 2, height >> 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[2]);
		glTexImage2D(GL_TEXTURE_2D, 3, GL_RGBA, width >> 3, height >> 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[3]);

		//glGenerateMipmap(GL_TEXTURE_2D);

		for (int m = 0; m < BSP_MIPLEVELS; m++)
			free(texdata[m]);

		free(texdata);
	}
}

void Wad::LoadDecals(const char* filename)
{
	loadBytes(filename, (char**)&whdr);

	for (int i = 0; i < whdr->numdirs; i++)
	{
		waddirentry_t* entry = (waddirentry_t*)((char*)whdr + whdr->diroffset) + i;

		GLuint texindex = AssetManager::getInst().setTexture(entry->name, "wad");

		int** texdata = (int**)malloc(sizeof(int*) * BSP_MIPLEVELS);
		int width, height;

		miptex_t* miptex = (miptex_t*)((char*)whdr + entry->entryoffset);

		loaddecalmiptex((char*)miptex, texdata, &width, &height);

		glBindTexture(GL_TEXTURE_2D, texindex);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifdef TEXTURE_FILTER
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif // TEXTURE_FILTER

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, BSP_MIPLEVELS - 1);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width >> 0, height >> 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[0]);
		glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA, width >> 1, height >> 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[1]);
		glTexImage2D(GL_TEXTURE_2D, 2, GL_RGBA, width >> 2, height >> 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[2]);
		glTexImage2D(GL_TEXTURE_2D, 3, GL_RGBA, width >> 3, height >> 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[3]);

		for (int m = 0; m < BSP_MIPLEVELS; m++)
			free(texdata[m]);

		free(texdata);
	}
}

GLuint Wad::LoadTexture(const char* filename, const char* texturename)
{
	GLuint texname = AssetManager::getInst().getTexture(texturename, "wad");
	if (texname != UINT32_MAX)
		return texname;

	std::string lowerfilename(texturename);
	for (int i = 0; i < lowerfilename.size(); i++)
		lowerfilename[i] = std::tolower(lowerfilename[i]);

	FILE* fileptr;
	fileptr = fopen(filename, "rb");

	wadheader_t phdr;
	fread(&phdr, sizeof(wadheader_t), 1, fileptr);

	fseek(fileptr, phdr.diroffset, SEEK_SET);

	waddirentry_t foundentry;
	bool didfind = false;
	for (int i = 0; i < phdr.numdirs; i++)
	{
		waddirentry_t entry;
		fread(&entry, sizeof(waddirentry_t), 1, fileptr);

		std::string lowerentry(entry.name);
		for (int i = 0; i < lowerentry.size(); i++)
			lowerentry[i] = std::tolower(lowerentry[i]);

		if (lowerfilename == lowerentry)
		{
			foundentry = entry;
			didfind = true;
			break;
		}
	}

	if (!didfind)
	{
		printf("*WARNING* Texture \"%s\" wasn't found in wad \"%s\"!\n", texturename, filename);
		fclose(fileptr);
		return UINT32_MAX;
	}

	miptex_t miptex;
	fseek(fileptr, foundentry.entryoffset, SEEK_SET);
	fread(&miptex, sizeof(miptex_t), 1, fileptr);
	uint32_t texdatasize = 0;
	for (int i = 0; i < BSP_MIPLEVELS; i++)
		texdatasize += (miptex.width >> i) * (miptex.height >> i);

	fseek(fileptr, texdatasize, SEEK_CUR);
	miptexpalette_t pal;
	fread(&pal, sizeof(miptexpalette_t), 1, fileptr);

	int moderncolors[MIPTEX_MAX_PAL];
	for (int i = 0; i < MIPTEX_MAX_PAL; i++)
	{
		int col = 0;
		col |= pal.colors[i].r <<  0;
		col |= pal.colors[i].g <<  8;
		col |= pal.colors[i].b << 16;

		if (col == 0x000000FF)
			col = 0;
		else
			col |= 0xFF000000;

		moderncolors[i] = col;
	}

	fseek(fileptr, foundentry.entryoffset + sizeof(miptex_t), SEEK_SET);
	std::vector<int> texdata[BSP_MIPLEVELS];
	for (int i = 0; i < BSP_MIPLEVELS; i++)
	{
		std::vector<ubyte_t> indexdata((miptex.width >> i) * (miptex.height >> i));
		fread(indexdata.data(), sizeof(ubyte_t), indexdata.size(), fileptr);
		
		for (int j = 0; j < indexdata.size(); j++)
			texdata[i].push_back(moderncolors[indexdata[j]]);
	}
	fclose(fileptr);

	texname = AssetManager::getInst().setTexture(texturename, "wad");

	glBindTexture(GL_TEXTURE_2D, texname);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifdef TEXTURE_FILTER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif // TEXTURE_FILTER

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, BSP_MIPLEVELS - 1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, miptex.width >> 0, miptex.height >> 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[0].data());
	glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA, miptex.width >> 1, miptex.height >> 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[1].data());
	glTexImage2D(GL_TEXTURE_2D, 2, GL_RGBA, miptex.width >> 2, miptex.height >> 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[2].data());
	glTexImage2D(GL_TEXTURE_2D, 3, GL_RGBA, miptex.width >> 3, miptex.height >> 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[3].data());

	return texname;
}