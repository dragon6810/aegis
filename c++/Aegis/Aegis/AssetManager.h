#pragma once
class AssetManager
{
public:
	static AssetManager& getInst()
	{
		static AssetManager instance;
		return instance;
	}

	// Source would be the name of the mdl, wad, bsp, etc.
	int getTextureIndex(char* texture, char* source);

	void cleanup();
private:
	AssetManager() {}

	AssetManager(AssetManager const&);
	void operator=(AssetManager const&);

	char** texturenames;
	int numtextures = 0;
};

