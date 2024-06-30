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

	printf("Mpeg 3 file \"%s\":\n", path.c_str());
	
	char frameid[4];
	uint32_t framesize;
	uint16_t frameflags;
	ubyte_t textformat;
	do
	{
		fread(frameid, 1, 4, ptr);
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
			std::string track;
			fread(&textformat, sizeof(ubyte_t), 1, ptr);
			track = std::string(framesize, 0);
			fread(&track[0], 1, framesize - 1, ptr);

			printf("\tTrack: %s\n", track.c_str());

			continue;
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