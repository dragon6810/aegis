#include "Waveform.h"

#include <stdio.h>

waveform_t Waveform::LoadSound(std::string path)
{
	waveform_t waveform{0, 0, nullptr};

	wavheader_t header;
	wavpcmfmt_t format;
	wadfact_t fact;
	waddata_t data;

	FILE* fileptr;
	fileptr = fopen(path.c_str(), "rb");
	fread(&header, sizeof(wavheader_t), 1, fileptr);


	fclose(fileptr);
	return waveform;
}