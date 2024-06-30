#include "Mpeg3.h"


mpeg3_t Mpeg3::LoadCD(std::string path)
{
	FILE* ptr;
	id3v2_t id3;
	bool unsync;
	bool extendhdr;
	bool experimental;
	bool isfooter;
	uint32_t size;
	mpeg3_t mpeg{};

	ptr = fopen(path.c_str(), "rb");
	if (ptr == nullptr)
	{
		printf("*WARNING* Mpeg3 file \"%s\" can not be found!\n", path.c_str());
		return {};
	}

	fread(&id3, sizeof(id3v2_t), 1, ptr);

	if (id3.flags & 0x0F)
	{
		printf("*WARNING* Bad Mpeg3 file \"%s\"\n", path.c_str());
		goto cleanup;
	}

	unsync = id3.flags & 0x80;
	extendhdr = id3.flags & 0x40;
	experimental = id3.flags & 0x20;
	isfooter = id3.flags & 0x10;
	size = SyncSafeToInt(id3.size);

	if (extendhdr)
		fseek(ptr, 6, SEEK_CUR); // Skip the extended header if there is one

	printf("Mpeg 3 file \"%s\" information:\n", path.c_str());
	
	char frameid[4];
	uint32_t framesize;
	uint16_t frameflags;
	ubyte_t textformat;
	do
	{
		fread(frameid, 1, 4, ptr);
		if (frameid[0] == 0)
		{
			fseek(ptr, -3, SEEK_CUR);
			continue;
		}
		fread(&framesize, sizeof(uint32_t), 1, ptr);
		framesize = SyncSafeToInt(framesize);
		fread(&frameflags, sizeof(uint16_t), 1, ptr);

		if ((frameid[0] == 'T') && (frameid[1] == 'P') && (frameid[2] == 'E') && (frameid[3] == '1'))
		{
			fread(&textformat, sizeof(ubyte_t), 1, ptr);
			mpeg.artist = std::string(framesize, 0);
			fread(&mpeg.artist[0], 1, framesize - 1, ptr);

			printf("\tArtist: %s\n", mpeg.artist.c_str());

			continue;
		}
		if ((frameid[0] == 'T') && (frameid[1] == 'A') && (frameid[2] == 'L') && (frameid[3] == 'B'))
		{
			fread(&textformat, sizeof(ubyte_t), 1, ptr);
			mpeg.album = std::string(framesize, 0);
			fread(&mpeg.album[0], 1, framesize - 1, ptr);

			printf("\tAlbum: %s\n", mpeg.album.c_str());

			continue;
		}
		if ((frameid[0] == 'T') && (frameid[1] == 'I') && (frameid[2] == 'T') && (frameid[3] == '2'))
		{
			fread(&textformat, sizeof(ubyte_t), 1, ptr);
			mpeg.title = std::string(framesize, 0);
			fread(&mpeg.title[0], 1, framesize - 1, ptr);

			printf("\tTitle: %s\n", mpeg.title.c_str());

			continue;
		}
		if ((frameid[0] == 'C') && (frameid[1] == 'O') && (frameid[2] == 'M') && (frameid[3] == 'M'))
		{
			fread(&textformat, sizeof(ubyte_t), 1, ptr);
			mpeg.comment = std::string(framesize, 0);
			fread(&mpeg.comment[0], 1, framesize - 1, ptr);

			printf("\tComment: %s\n", mpeg.comment.c_str());

			continue;
		}
		if ((frameid[0] == 'T') && (frameid[1] == 'E') && (frameid[2] == 'N') && (frameid[3] == 'C'))
		{
			fread(&textformat, sizeof(ubyte_t), 1, ptr);
			mpeg.encodedby = std::string(framesize, 0);
			fread(&mpeg.encodedby[0], 1, framesize - 1, ptr);

			printf("\tEncoded By: %s\n", mpeg.encodedby.c_str());

			continue;
		}
		if ((frameid[0] == 'M') && (frameid[1] == 'C') && (frameid[2] == 'D') && (frameid[3] == 'I'))
		{
			fseek(ptr, 804, SEEK_CUR);

			continue;
		}
		if ((frameid[0] == 'T') && (frameid[1] == 'R') && (frameid[2] == 'C') && (frameid[3] == 'K'))
		{
			fread(&textformat, sizeof(ubyte_t), 1, ptr);
			mpeg.trackindex = std::string(framesize, 0);
			fread(&mpeg.trackindex[0], 1, framesize - 1, ptr);

			printf("\tTrack: %s\n", mpeg.trackindex.c_str());

			continue;
		}
		if ((frameid[0] == 'T') && (frameid[1] == 'C') && (frameid[2] == 'O') && (frameid[3] == 'N'))
		{
			fread(&textformat, sizeof(ubyte_t), 1, ptr);
			mpeg.contenttype = std::string(framesize, 0);
			fread(&mpeg.contenttype[0], 1, framesize - 1, ptr);

			printf("\tContent Type: %s\n", mpeg.contenttype.c_str());

			continue;
		}
		if ((frameid[0] == 'T') && (frameid[1] == 'L') && (frameid[2] == 'E') && (frameid[3] == 'N'))
		{
			fread(&textformat, sizeof(ubyte_t), 1, ptr);
			std::string text;
			text = std::string(framesize, 0);
			fread(&text[0], 1, framesize - 1, ptr);
			mpeg.length = std::stoi(text);

			uint32_t milliseconds = mpeg.length % 1000;
			uint32_t seconds = mpeg.length / 1000;
			uint32_t minutes = seconds / 60;
			seconds = seconds % 60;
			uint32_t hours = minutes / 60;
			minutes = minutes % 60;

			printf("\tLength: %dh:%dm:%ds:%dms\n", hours, minutes, seconds, milliseconds);

			continue;
		}
		if ((frameid[0] == 'X') && (frameid[1] == 'i') && (frameid[2] == 'n') && (frameid[3] == 'g'))
		{
			fseek(ptr, -4 - sizeof(uint32_t) - sizeof(uint16_t), SEEK_CUR);

			fread(mpeg.xing.id, sizeof(mpeg.xing.id), 1, ptr);
			fread(&mpeg.xing.flags, sizeof(mpeg.xing.flags), 1, ptr);

			if (mpeg.xing.flags & 0x01000000)
			{
				fread(&mpeg.xing.frames, sizeof(mpeg.xing.frames), 1, ptr);
				mpeg.xing.frames = SwapEndian(mpeg.xing.frames);
			}
			if (mpeg.xing.flags & 0x02000000)
			{
				fread(&mpeg.xing.bytes, sizeof(mpeg.xing.bytes), 1, ptr);
				mpeg.xing.bytes = SwapEndian(mpeg.xing.bytes);
			}
			if (mpeg.xing.flags & 0x04000000)
			{
				fread(mpeg.xing.toc, 1, sizeof(mpeg.xing.toc), ptr);
			}
			if (mpeg.xing.flags & 0x08000000)
			{
				fread(&mpeg.xing.quality, sizeof(mpeg.xing.quality), 1, ptr);
				mpeg.xing.quality = SwapEndian(mpeg.xing.quality);
				printf("\tQuality: %d/100\n", mpeg.xing.quality);
			}

			mpeg.xing.lametag = std::string(10, 0);
			fread(&mpeg.xing.lametag[0], 1, 9, ptr);
			printf("\tLame Tag: \"%s\"", mpeg.xing.lametag.c_str());

			continue;
		}
		else
		{
			fseek(ptr, -4 - sizeof(uint32_t) - sizeof(uint16_t) + 1, SEEK_CUR);
		}
	} while (true);

	cleanup:
	fclose(ptr);
	return mpeg;
}

uint32_t Mpeg3::SyncSafeToInt(uint32_t syncsafe)
{
	uint32_t byte0;
	uint32_t byte1;
	uint32_t byte2;
	uint32_t byte3;

	byte0 = (uint32_t)(syncsafe & 0x000000FF) >>  0;
	byte1 = (uint32_t)(syncsafe & 0x0000FF00) >>  8;
	byte2 = (uint32_t)(syncsafe & 0x00FF0000) >> 16;
	byte3 = (uint32_t)(syncsafe & 0xFF000000) >> 24;

	return (byte0 << 21 | byte1 << 14 | byte2 << 7 | byte3);
}

uint32_t Mpeg3::SwapEndian(uint32_t n)
{
	uint32_t swapped;

	swapped = 0;
	swapped |= (uint32_t)(n & 0xFF000000) >> 24;
	swapped |= (uint32_t)(n & 0x00FF0000) >>  8;
	swapped |= (uint32_t)(n & 0x0000FF00) <<  8;
	swapped |= (uint32_t)(n & 0x000000FF) << 24;

	return swapped;
}

/*
sf::Sound Mpeg3::LoadFrame(FILE* frame)
{

}
*/