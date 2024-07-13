#include "Targa.h"

#include <stdio.h>

#include <vector>

#include "Print.h"
#include "AssetManager.h"

#include "defs.h"

#include "binaryloader.h"

GLuint Targa::LoadTargaImage(std::string path, int* width, int* height)
{
	int i;
	int j;

	FILE* ptr;

	// Header
	ubyte_t idsize;
	ubyte_t cmap;		  // Color map, not character map!
	ubyte_t imgtype;
	uint16_t ifirstentry; // Color map stuff; the first color map index
	uint16_t nentries;	  // Color map stuff; the number of color map entries
	ubyte_t entrybits;	  // Color map stuff; the number of bits per entry (e.g. 8, 24, etc.)
	uint16_t xoffs;
	uint16_t yoffs;
	uint16_t imgwidth;
	uint16_t imgheight;
	ubyte_t pixeldepth;	  // Bit depth of each pixel in the image, whether it's an index or true color'
	ubyte_t imgdesc;

	ubyte_t pixelsize;    // Size, in bytes, of each pixel

	// Image ID
	char imgid[256];

	// Image Data
	std::vector<char> texdata;

	// Developer Data
	uint16_t ndevtags;

	// Extension Area (If Included)
	uint16_t extsize;
	char author[41];
	char comment[324];
	uint16_t date[6];	      // Month/Day/Year/Hour/Minute/Second
	char job[41];
	uint16_t jobtime[3];      // Hours/Minutes/Seconds
	char software[41];
	uint16_t softwarever;
	char softwareletter;
	color32_t keycolor;
	uint16_t pixelnum;        // Used for calculating the aspect ratio of pixels; Numerator
	uint16_t pixelden;		  // Used for calculating the aspect ratio of pixels; Denominator
	uint16_t gammanum;        // Used for calculating the gamma correction value; Numerator
	uint16_t gammaden;        // Used for calculating the gamma correction value; Denominator
	uint32_t colorcorrectoffs; 
	uint32_t postageoffs;
	uint32_t scanlineoffs;
	ubyte_t attribstype;

	// Footer
	uint32_t extoffs;
	uint32_t devoffs;
	char endascii[18];

	bool newf = false;

	GLuint texname;

	if ((ptr = fopen(path.c_str(), "rb")) == nullptr)
		return 0xFFFFFFFF;

	// Quickly check if the file is version 1.0 or 2.0 by checking for the signature
	fseek(ptr, -1 * sizeof(endascii), SEEK_END);
	fread(endascii, 1, sizeof(endascii), ptr);
	if (!strcmp(endascii, "TRUEVISION-XFILE."))
		newf = true;
	fseek(ptr, -1 * sizeof(endascii) - 8, SEEK_CUR);
	fread(&extoffs, sizeof(extoffs), 1, ptr);
	LittleEndian(&extoffs, sizeof(extoffs));
	fread(&devoffs, sizeof(devoffs), 1, ptr);
	LittleEndian(&devoffs, sizeof(devoffs));

	fseek(ptr, 0, SEEK_SET);
	fread(&idsize, sizeof(idsize), 1, ptr);
	fread(&cmap, sizeof(cmap), 1, ptr);
	fread(&imgtype, sizeof(imgtype), 1, ptr);
	fread(&ifirstentry, sizeof(ifirstentry), 1, ptr);
	LittleEndian(&ifirstentry, sizeof(ifirstentry));
	fread(&nentries, sizeof(nentries), 1, ptr);
	LittleEndian(&nentries, sizeof(nentries));
	fread(&entrybits, sizeof(entrybits), 1, ptr);
	fread(&xoffs, sizeof(xoffs), 1, ptr);
	LittleEndian(&xoffs, sizeof(xoffs));
	fread(&yoffs, sizeof(yoffs), 1, ptr);
	LittleEndian(&yoffs, sizeof(yoffs));
	fread(&imgwidth, sizeof(imgwidth), 1, ptr);
	LittleEndian(&imgwidth, sizeof(imgwidth));
	fread(&imgheight, sizeof(imgheight), 1, ptr);
	LittleEndian(&imgheight, sizeof(imgheight));
	fread(&pixeldepth, sizeof(pixeldepth), 1, ptr);
	fread(&imgdesc, sizeof(imgdesc), 1, ptr);
	//fseek(ptr, 4, SEEK_CUR);

	pixelsize = (pixeldepth / 24) * 8; // Min(Bit depth / 3, 8)

	if (cmap != 0)
	{
		Print::Aegis_Warning("Targa image file %s uses unsupported colormap mode %d!\n", path.c_str(), cmap);
		return 0xFFFFFFFF;
	}

	memset(imgid, 0, sizeof(imgid));
	fread(imgid, 1, idsize, ptr);

	// TODO: Read Color Map

	// TODO: Read Image Data
	if ((pixeldepth != 24) && (pixeldepth != 32))
	{
		Print::Aegis_Warning("Targa image file %s uses unsupported pixel depth %d!\n", path.c_str(), pixeldepth);
		return 0xFFFFFFFF;
	}

	texdata.resize((imgwidth * imgheight) * pixelsize);
	for (i = 0; i < ((imgwidth * imgheight) * pixelsize); i += pixelsize)
	{
		for(j = 0; j < pixelsize; j++)
			fread(&texdata[i + j], 1, 1, ptr);
	}

	if (devoffs != 0 && newf)
	{
		fseek(ptr, devoffs, SEEK_SET);
		fread(&ndevtags, sizeof(ndevtags), 1, ptr);
		LittleEndian(&ndevtags, sizeof(ndevtags));

		// TODO: Read Developer Tags

		fseek(ptr, ndevtags * 10, SEEK_CUR); // Skip over the tags, for now.
	}

	if (extoffs != 0 && newf)
	{
		fseek(ptr, extoffs, SEEK_SET);
		fread(&extsize, sizeof(extsize), 1, ptr);
		LittleEndian(&extsize, sizeof(extsize));

		if (extsize != 495)
		{
			fseek(ptr, extsize, SEEK_CUR); // Skip over the extension area, unsupported format
		}
		else
		{
			fread(author, 1, sizeof(author), ptr);
			fread(comment, 1, sizeof(comment), ptr);
			for (i = 0; i < 6; i++)
			{
				fread(&date[i], sizeof(date[i]), 1, ptr);
				LittleEndian(&date[i], sizeof(date[i]));
			}
			fread(job, 1, sizeof(job), ptr);
			for (i = 0; i < 3; i++)
			{
				fread(&jobtime[i], sizeof(jobtime[i]), 1, ptr);
				LittleEndian(&jobtime[i], sizeof(jobtime[i]));
			}
			fread(software, 1, sizeof(software), ptr);
			fread(&softwarever, sizeof(softwarever), 1, ptr);
			LittleEndian(&softwarever, sizeof(softwarever));
			fread(&softwareletter, 1, 1, ptr);
			fread(&keycolor, sizeof(keycolor), 1, ptr);
			LittleEndian(&keycolor, sizeof(keycolor));
			fread(&pixelnum, sizeof(pixelnum), 1, ptr);
			LittleEndian(&pixelnum, sizeof(pixelnum));
			fread(&pixelden, sizeof(pixelden), 1, ptr);
			LittleEndian(&pixelden, sizeof(pixelden));
			fread(&gammanum, sizeof(gammanum), 1, ptr);
			LittleEndian(&gammanum, sizeof(gammanum));
			fread(&gammaden, sizeof(gammaden), 1, ptr);
			LittleEndian(&gammaden, sizeof(gammaden));
			fread(&colorcorrectoffs, sizeof(colorcorrectoffs), 1, ptr);
			LittleEndian(&colorcorrectoffs, sizeof(colorcorrectoffs));
			fread(&postageoffs, sizeof(postageoffs), 1, ptr);
			LittleEndian(&postageoffs, sizeof(postageoffs));
			fread(&scanlineoffs, sizeof(scanlineoffs), 1, ptr);
			LittleEndian(&scanlineoffs, sizeof(scanlineoffs));
			fread(&attribstype, sizeof(attribstype), 1, ptr);
		}
	}

	texname = AssetManager::getInst().setTexture(path.c_str(), "targa");
	
	glBindTexture(GL_TEXTURE_2D, texname);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifdef TEXTURE_FILTER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif // TEXTURE_FILTER

	switch (pixeldepth)
	{
	case 32:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgwidth, imgheight, 0, GL_BGRA, GL_UNSIGNED_BYTE, texdata.data());
	case 24:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgwidth, imgheight, 0, GL_BGR, GL_UNSIGNED_BYTE, texdata.data());
	default:
		break;
	}

	if (width != nullptr)
		*width = imgwidth;

	if (height != nullptr)
		*height = imgheight;

	return texname;
}