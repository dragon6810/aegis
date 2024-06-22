#pragma once

#include <stdint.h>
#include <string>

#define WAVE_FORMAT_PCM		   0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_ALAW	   0x0006
#define WAVE_FORMAT_MULAW	   0x0007
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

typedef struct
{
	char chunkid[4];    // Should be RIFF
	uint32_t chunksize;
	char waveid[4];     // Should be WAVE
} wavheader_t;

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
	uint16_t extensionsize;
	uint16_t validbitspersample;
	uint32_t channelmask;
	char subformat[16];
} wavpcmfmt_t;

typedef struct
{
	char chunkid[4];	  // Should be fact
	uint32_t chunksize;
	uint32_t samplecount; // Number of samples per channel
} wadfact_t;

typedef struct
{
	char chunkid[4];    // Should be data
	uint32_t chunksize;
	// Data goes here
	// Padding byte if n samples is odd
} waddata_t;

typedef struct
{
	uint32_t nsamples;
	uint32_t samplerate;
	char* data;
} waveform_t;

class Waveform
{
public:
	static waveform_t LoadSound(std::string path);
};

