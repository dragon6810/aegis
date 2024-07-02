#include "Mpeg3.h"


mpeg3_t Mpeg3::LoadCD(std::string path)
{
	int i;

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
			printf("\tEncoder: \"%s\"\n", mpeg.xing.lametag.c_str());

			if (mpeg.xing.lametag[0] != 'L' || mpeg.xing.lametag[1] != 'A' || mpeg.xing.lametag[2] != 'M' || mpeg.xing.lametag[3] != 'E')
			{
				printf("*WARNING* Mpeg 3 file \"%s\" uses unsopported encoder \"%s\"! Mpeg 3 files MUST use LAME!\n", path.c_str(), mpeg.xing.lametag.c_str());
				goto cleanup;
			}

			sscanf(mpeg.xing.lametag.c_str() + 4, "%u.%u", &mpeg.xing.major, &mpeg.xing.minor);

			ubyte_t infoandvbr;
			fread(&infoandvbr, sizeof(ubyte_t), 1, ptr);
			mpeg.xing.infotagrevision = infoandvbr >> 4;
			mpeg.xing.vbrmethod = infoandvbr & 0x0F;

			ubyte_t lowpassfixed;
			fread(&lowpassfixed, sizeof(ubyte_t), 1, ptr);
			mpeg.xing.lowpass = (float)lowpassfixed / 100.0;

			fread(&mpeg.xing.peak, sizeof(mpeg.xing.peak), 1, ptr);
			fread(&mpeg.xing.radiogain, sizeof(mpeg.xing.radiogain), 1, ptr);
			fread(&mpeg.xing.audiophilegain, sizeof(mpeg.xing.audiophilegain), 1, ptr);
			
			ubyte_t encodingandath;
			fread(&encodingandath, sizeof(ubyte_t), 1, ptr);
			mpeg.xing.encodingflags = encodingandath >> 4;
			mpeg.xing.athtype = encodingandath & 0x0F;

			if (mpeg.xing.vbrmethod == 2)
				fread(&mpeg.xing.specifiedbr, sizeof(mpeg.xing.specifiedbr), 1, ptr);
			else
				fread(&mpeg.xing.minimalbr, sizeof(mpeg.xing.minimalbr), 1, ptr);

			char encoderdelays[3];
			fread(encoderdelays, 1, sizeof(encoderdelays), ptr);
			mpeg.xing.encoderdelay = (encoderdelays[1] >> 4) | (encoderdelays[0] << 4);
			mpeg.xing.encoderpad = (encoderdelays[1] << 8) | encoderdelays[2];

			ubyte_t misc;
			fread(&misc, sizeof(misc), 1, ptr);
			mpeg.xing.noiseshaping = misc & 0x03;
			mpeg.xing.stereomode = misc & 0x1E >> 2;
			mpeg.xing.unwise = misc & 0x20;
			mpeg.xing.sourcesamplefreq = misc & 0xC0 >> 6;

			fread(&mpeg.xing.gain, sizeof(mpeg.xing.gain), 1, ptr);

			uint16_t presetandsurround;
			fread(&presetandsurround, sizeof(presetandsurround), 1, ptr);
			mpeg.xing.presetused = presetandsurround & 0x07FF;
			mpeg.xing.surroundinfo = (presetandsurround & 0x3800) >> 11;

			fread(&mpeg.xing.originlen, sizeof(mpeg.xing.originlen), 1, ptr);
			mpeg.xing.originlen = SwapEndian(mpeg.xing.originlen);

			fread(&mpeg.xing.crcstart, sizeof(mpeg.xing.crcstart), 1, ptr);
			fread(&mpeg.xing.crcend, sizeof(mpeg.xing.crcend), 1, ptr);

			fread(&mpeg.xing.infocrc, sizeof(mpeg.xing.infocrc), 1, ptr);

			break;
		}
		else
		{
			fseek(ptr, -4 - sizeof(uint32_t) - sizeof(uint16_t) + 1, SEEK_CUR);
		}
	} while (true);
	
	i = 0;
	while(i < mpeg.xing.frames)
	{
		uint32_t hdr;
		uint32_t samplerate; // hz
		bool padding;

		ubyte_t samplerateindex;
		ubyte_t channelmode;
		ubyte_t modeextension;
		bool copyright;
		bool original;

		uint32_t numframes;
		std::vector<float> framedata;
		do
		{
			fread(&hdr, sizeof(hdr), 1, ptr);
			hdr = SwapEndian(hdr);
			fseek(ptr, -1 * sizeof(hdr) + 1, SEEK_CUR);
		} while ((hdr & 0xFFE00000) != 0xFFE00000);
		fseek(ptr, sizeof(hdr) - 1, SEEK_CUR);

		ubyte_t version = (hdr & 0x00180000) >> 19;
		ubyte_t layer = (hdr & 0x00060000) >> 17;

		if (version == 1 || layer == 0)
		{
			fseek(ptr, -1 * sizeof(hdr) + 1, SEEK_CUR);
			continue;
		}

		switch (version)
		{
		case 0:
			version = 3;
			break;
		case 2:
			version = 2;
			break;
		case 3:
			version = 1;
			break;
		default:
			break;
		}
		layer = 3 - layer + 1;

		ubyte_t brindex = (hdr & 0x0000F000) >> 12;
		int kbps;
		switch (brindex)
		{
		case 0:
			goto skip; // Can't be bothered with free bitrates
			break;
		case 1:
			if (version == 1 || layer == 1)
				kbps = 32;
			else
				kbps = 8;
			break;
		case 2:
			if (version == 1 && layer == 1)
				kbps = 64;
			else if (version == 1 && layer == 2)
				kbps = 48;
			else if (version == 1 && layer == 3)
				kbps = 40;
			else if (version == 2 && layer == 1)
				kbps = 40;
			else
				kbps = 16;
			break;
		case 3:
			if (version == 1 && layer == 1)
				kbps = 96;
			else if (version == 1 && layer == 2)
				kbps = 56;
			else if (version == 1 && layer == 3)
				kbps = 48;
			else if (version == 2 && layer == 1)
				kbps = 56;
			else
				kbps = 24;
			break;
		case 4:
			if (version == 1 && layer == 1)
				kbps = 128;
			else if (version == 1 && layer == 2)
				kbps = 64;
			else if (version == 1 && layer == 3)
				kbps = 56;
			else if (version == 2 && layer == 1)
				kbps = 64;
			else
				kbps = 32;
			break;
		case 5:
			if (version == 1 && layer == 1)
				kbps = 160;
			else if (version == 1 && layer == 2)
				kbps = 80;
			else if (version == 1 && layer == 3)
				kbps = 64;
			else if (version == 2 && layer == 1)
				kbps = 80;
			else
				kbps = 40;
			break;
		case 6:
			if (version == 1 && layer == 1)
				kbps = 192;
			else if (version == 1 && layer == 2)
				kbps = 96;
			else if (version == 1 && layer == 3)
				kbps = 80;
			else if (version == 2 && layer == 1)
				kbps = 96;
			else
				kbps = 48;
			break;
		case 7:
			if (version == 1 && layer == 1)
				kbps = 224;
			else if (version == 1 && layer == 2)
				kbps = 112;
			else if (version == 1 && layer == 3)
				kbps = 96;
			else if (version == 2 && layer == 1)
				kbps = 112;
			else
				kbps = 56;
			break;
		case 8:
			if (version == 1 && layer == 1)
				kbps = 256;
			else if (version == 1 && layer == 2)
				kbps = 128;
			else if (version == 1 && layer == 3)
				kbps = 112;
			else if (version == 2 && layer == 1)
				kbps = 128;
			else
				kbps = 64;
			break;
		case 9:
			if (version == 1 && layer == 1)
				kbps = 288;
			else if (version == 1 && layer == 2)
				kbps = 160;
			else if (version == 1 && layer == 3)
				kbps = 128;
			else if (version == 2 && layer == 1)
				kbps = 144;
			else
				kbps = 80;
			break;
		case 10:
			if (version == 1 && layer == 1)
				kbps = 320;
			else if (version == 1 && layer == 2)
				kbps = 192;
			else if (version == 1 && layer == 3)
				kbps = 160;
			else if (version == 2 && layer == 1)
				kbps = 160;
			else
				kbps = 96;
			break;
		case 11:
			if (version == 1 && layer == 1)
				kbps = 352;
			else if (version == 1 && layer == 2)
				kbps = 224;
			else if (version == 1 && layer == 3)
				kbps = 192;
			else if (version == 2 && layer == 1)
				kbps = 176;
			else
				kbps = 112;
			break;
		case 12:
			if (version == 1 && layer == 1)
				kbps = 384;
			else if (version == 1 && layer == 2)
				kbps = 256;
			else if (version == 1 && layer == 3)
				kbps = 224;
			else if (version == 2 && layer == 1)
				kbps = 192;
			else
				kbps = 128;
			break;
		case 13:
			if (version == 1 && layer == 1)
				kbps = 416;
			else if (version == 1 && layer == 2)
				kbps = 320;
			else if (version == 1 && layer == 3)
				kbps = 256;
			else if (version == 2 && layer == 1)
				kbps = 224;
			else
				kbps = 144;
			break;
		case 14:
			if (version == 1 && layer == 1)
				kbps = 448;
			else if (version == 1 && layer == 2)
				kbps = 384;
			else if (version == 1 && layer == 3)
				kbps = 320;
			else if (version == 2 && layer == 1)
				kbps = 256;
			else
				kbps = 160;
			break;
		case 15:
			goto skip; // Bad bitrate
			break;
		default:
			break;
		}
		
		if (layer == 2)
		{
			if (kbps < 64 && (mpeg.xing.stereomode == 1 || mpeg.xing.stereomode == 2 || mpeg.xing.stereomode == 6))
				goto skip;
			if (kbps == 80 && (mpeg.xing.stereomode == 1 || mpeg.xing.stereomode == 2 || mpeg.xing.stereomode == 6))
				goto skip;
			if (kbps > 192 && mpeg.xing.stereomode == 0)
				goto skip;
		}

		samplerateindex = (hdr & 0x00000C00) >> 10;
		switch (samplerateindex)
		{
		case 0:
			switch (version)
			{
			case 1:
				samplerate = 44100;
				break;
			case 2:
				samplerate = 22050;
				break;
			case 3:
				samplerate = 11025;
				break;
			default:
				break;
			}
			break;
		case 1:
			switch (version)
			{
			case 1:
				samplerate = 48000;
				break;
			case 2:
				samplerate = 24000;
				break;
			case 3:
				samplerate = 12000;
				break;
			default:
				break;
			}
			break;
		case 2:
			switch (version)
			{
			case 1:
				samplerate = 32000;
				break;
			case 2:
				samplerate = 16000;
				break;
			case 3:
				samplerate = 8000;
				break;
			default:
				break;
			}
			break;
		case 3:
			goto skip;
			break;
		default:
			break;
		}

		padding = hdr & 0x00000200;

		channelmode = (hdr & 0x000000C0) >> 6;
		modeextension = (hdr & 0x00000030) >> 4;

		copyright = hdr & 0x00000008;
		original = hdr & 0x00000004;

		numframes = 144 * kbps / samplerate;

		i++;

	skip:
		continue;
	}

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