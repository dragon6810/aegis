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

