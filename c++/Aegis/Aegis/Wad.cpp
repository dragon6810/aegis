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