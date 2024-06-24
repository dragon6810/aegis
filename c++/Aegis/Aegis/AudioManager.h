#pragma once

#include <thread>
#include <unordered_map>

#include "defs.h"
#include "Waveform.h"

#define AUDIO_NUMCHANNELS 8

typedef struct
{
	std::thread thread;
	bool playing;
	int importance;
	float timeleft;
	vec3_t position; // Relative to camera
	float pitch;
	float volume;
	waveform_t sound;
} audiochannel_t;

class AudioManager
{
public:
	AudioManager();
	~AudioManager();

	void Initialize();
	void UnloadSounds();

	audiochannel_t* GetChannel(int index);

	int PlaySound(std::string sound, int importance);
	int PlaySound(std::string sound, int importance, vec3_t position);
private:
	audiochannel_t channels[AUDIO_NUMCHANNELS];
	std::unordered_map<std::string, waveform_t> waveforms;

	void PlaySoundOnChannel(int channel, waveform_t sound, vec3_t position, float pitch, float volume);
};