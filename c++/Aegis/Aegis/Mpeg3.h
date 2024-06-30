#pragma once

#include <stdio.h>

#include <vector>
#include <memory>

#include <SFML/Audio.hpp>

#include "defs.h"

// https://mutagen-specs.readthedocs.io/en/latest/id3/id3v2.4.0-structure.html
#pragma pack(push, 1)
typedef struct
{
	char tag[3];     // Should be TAG
	uint16_t version;
	ubyte_t flags;
	uint32_t size;   // SynchSafe
} id3v2_t;

typedef struct
{
	uint32_t starttime; // Index to frame of mpeg3
	char pad[4];
} trackdescriptor_t;

struct xing_t
{
	char id[4];
	uint32_t flags;
	uint32_t frames;
	uint32_t bytes;
	ubyte_t toc[100];
	uint32_t quality;

	std::string lametag;
	uint32_t major;
	uint32_t minor;
	ubyte_t infotagrevision;
	ubyte_t vbrmethod;
	float lowpass;
	float peak;
	short radiogain;
	short audiophilegain;
	ubyte_t encodingflags;
	ubyte_t athtype;
	ubyte_t specifiedbr;
	ubyte_t minimalbr;
	uint16_t encoderdelay;
	uint16_t encoderpad;
	ubyte_t noiseshaping;
	ubyte_t stereomode;
	bool unwise;
	ubyte_t sourcesamplefreq;
	byte_t gain;
	uint16_t presetused;
	ubyte_t surroundinfo;
	uint32_t originlen;       // Length of the file in bytes given to LAME to be encoded
	ubyte_t crcstart;		  // The first byte of the first media frame
	ubyte_t crcend;			  // The last byte of the last media frame
	uint16_t infocrc;
};

typedef struct
{
	std::string artist;
	std::string album;
	std::string title;
	std::string comment;
	std::string encodedby;
	std::string trackindex;
	std::string contenttype;
	uint32_t length;         // Milliseconds
	xing_t xing;
	std::vector<sf::Sound> frames;
} mpeg3_t;

class Mpeg3
{
public:
	static mpeg3_t LoadCD(std::string path);
	static uint32_t SyncSafeToInt(uint32_t syncsafe);
	static uint32_t SwapEndian(uint32_t n);
private:
	static sf::Sound LoadFrame(FILE* frame);
};

