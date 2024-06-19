#pragma once

#include <vector>

#include <GL/glew.h>

#define SKYBOX_NUMIMAGES 6
#define SKYBOX_PALETTESIZE 256

#pragma pack(push, 1)
typedef struct
{
	char ftype[2];		   // Should be BM
	uint32_t fsize;		   // File size in bytes
	uint16_t reserved1;	   // 0
	uint16_t reserved2;	   // 0
	uint32_t bitmapoffset; // Offset in bytes from the beginning of the file to the bitmap
} bitmapheader_t;

typedef struct
{
	uint32_t infohdrsize;     // Always 40
	int width;                // Width of bitmap in pixels
	int height;               // Height of bitmap in pixels
	uint16_t planes;          // Always 1
	uint16_t bitdepth;        // Bits per pixel, MUST BE 8 FOR AEGIS
	uint32_t compression;     // Compression algorithm used
	uint32_t imagesize;       // Size in bytes of image - 0 for uncompressed RGB
	int pixelspermeterx;      // We don't need to worry about this
	int pixelspermetery;      // We don't need to worry about this
	uint32_t colorsused;	  // Colors in the bitmap that are actually used
	uint32_t importantcolors; // The number of "important" colors, bare minimum. 0 if all matter
} bitmapinfoheader_t;

typedef struct
{
	char b;
	char g;
	char r;
	char reserved;
}bitmapcolor_t;

class Skybox
{
public:
	void LoadSky(char* sky);

	~Skybox();
private:
	std::vector<int> LoadBitmap(char* path, int whichside, int* width, int* height);

	bitmapheader_t* mhdr[SKYBOX_NUMIMAGES];
	GLuint texnames[SKYBOX_NUMIMAGES];
};

