#include "Waveform.h"

#include <stdio.h>

waveform_t Waveform::LoadSound(std::string path)
{
	waveform_t waveform{};

	wavheader_t header;
	wavpcmfmt_t format;
	wadfact_t fact;
	waddata_t data;

	FILE* fileptr;
	fileptr = fopen(path.c_str(), "rb");
	if (fileptr == nullptr)
	{
		printf("*WARNING* File path %s does not exist!\n", path.c_str());
		waveform.duration = 0.0;
		waveform.nsamples = 0;
		waveform.samplerate = 0;
		waveform.sound = std::make_unique<sf::SoundBuffer>();
		short zero = 0;
		waveform.sound->loadFromSamples(&zero, 1, 1, 1);
		return waveform;
	}

	fread(&header, sizeof(wavheader_t), 1, fileptr);
	fread(&format, sizeof(wavpcmfmt_t), 1, fileptr);
	if (format.chunksize != 16)
		fseek(fileptr, format.chunksize - 16, SEEK_CUR);
	printf("Size: %d.\n", sizeof(wavpcmfmt_t));

	waveform.samplerate = format.samplespersecond;

	fread(&data, sizeof(waddata_t), 1, fileptr);

	printf("Waveform file \"%s\" information:\n", path.c_str());
	printf("\t %d channels\n", format.nchannels);
	printf("\t %dhz\n", format.samplespersecond);
	printf("\t Bit depth: %d\n", format.bitdepth);

	int realsize;
	if (data.chunksize & 1)
		realsize = data.chunksize - 1;
	else
		realsize = data.chunksize;

	std::vector<unsigned char> bytedata(realsize);
	std::vector<short> sounddata(realsize);
	int taper = 256;

	if (format.bitdepth != 8)
	{
		printf("*WARNING* Aegis currently only supports waveform files with a bit depth of 8. File \"%s\" has a bit depth of %d!\n", path.c_str(), format.bitdepth);
		goto cleanup;
	}

	waveform.nsamples = realsize;
	waveform.duration = (float)waveform.nsamples / (float)waveform.samplerate;
	fread(bytedata.data(), sizeof(char), realsize, fileptr);
	
	for (int i = 0; i < waveform.nsamples; i++)
	{
		sounddata[i] = bytedata[i] << 6;
		if (i >= waveform.nsamples - taper)
		{
			sounddata[i] = (float)sounddata[i] * ((float)(waveform.nsamples - i) / (float)taper);
		}
		else if (i <= taper)
		{
			sounddata[i] = (float)sounddata[i] * ((float) i / (float)taper);
		}
	}

	waveform.sound = std::make_unique<sf::SoundBuffer>();
	
	if (!waveform.sound->loadFromSamples(sounddata.data(), waveform.nsamples, 1, waveform.samplerate))
	{
		printf("*WARNING* Failed to create Sound Buffer Object from sound \"%s\"!\n", path.c_str());
		goto cleanup;
	}

	cleanup:
	fclose(fileptr);
	return waveform;
}

void Waveform::PlaySound(waveform_t sound)
{
	sf::Sound s;
	s.setBuffer(*sound.sound);
	s.play();
	sf::sleep(sf::seconds(sound.duration));
}