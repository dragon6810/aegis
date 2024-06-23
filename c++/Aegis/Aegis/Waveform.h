#pragma once

#include <stdint.h>
#include <string>
#include <memory>

#include <SFML/Audio.hpp>

#define WAVE_FORMAT_PCM		   0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_ALAW	   0x0006
#define WAVE_FORMAT_MULAW	   0x0007
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

#pragma pack(push, 1)
typedef struct
{
	char chunkid[4];    // Should be RIFF
	uint32_t chunksize;
	char waveid[4];     // Should be WAVE
} wavheader_t;

#pragma pack(push, 1)
typedef struct
{
	char chunkid[4];			 // Should be fmt
	uint32_t chunksize;
	short formatcode;			 // Should be WAVE_FORMAT_PCM
	uint16_t nchannels;
	uint32_t samplespersecond;   // Blocks per second
	uint32_t bytespersecond;
	uint16_t blocksize;			 // Block size, in bytes
	uint16_t bitdepth;           // Bits per sample
} wavpcmfmt_t;

#pragma pack(push, 1)
typedef struct
{
	char chunkid[4];	  // Should be fact
	uint32_t chunksize;
	uint32_t samplecount; // Number of samples per channel
} wadfact_t;

#pragma pack(push, 1)
typedef struct
{
	char chunkid[4];    // Should be data
	uint32_t chunksize;
	// Data goes here
	// Padding byte if n samples is odd
} waddata_t;

struct waveform_t
{
	uint32_t nsamples;
	uint32_t samplerate;
	float duration;
	std::unique_ptr<sf::SoundBuffer> sound;

	waveform_t() : nsamples(0), samplerate(0), duration(0.0f), sound(nullptr) {}

	waveform_t(const waveform_t& other) : nsamples(other.nsamples), samplerate(other.samplerate), duration(other.duration) 
	{
		if (other.sound)
			sound = std::make_unique<sf::SoundBuffer>(*other.sound);
	}

	waveform_t& operator=(const waveform_t& a)
	{
		nsamples = a.nsamples;
		samplerate = a.samplerate;
		duration = a.duration;
		sound = std::make_unique<sf::SoundBuffer>(*a.sound);
		return *this;
	}
};

class Waveform
{
public:
	static waveform_t LoadSound(std::string path);
	static void PlaySound(waveform_t sound);
};

