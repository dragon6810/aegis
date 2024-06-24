#include "AudioManager.h"

#include "Quaternion.h"
#include "Game.h"

AudioManager::AudioManager()
{
	Initialize();
}

AudioManager::~AudioManager()
{
	for (int i = 0; i < AUDIO_NUMCHANNELS; i++)
	{
		channels[i].sound.sound.release();
		if (channels[i].thread.joinable())
			channels[i].thread.join();
	}
}

void AudioManager::Initialize()
{
	for (int i = 0; i < AUDIO_NUMCHANNELS; i++)
	{
		channels[i].playing = false;
	}
}

void AudioManager::UnloadSounds()
{
	waveforms.erase(waveforms.begin());
}

audiochannel_t* AudioManager::GetChannel(int index)
{
	return &channels[index];
}

int AudioManager::PlaySound(std::string sound, int importance)
{
	return PlaySound(sound, importance, { 0, 0, 0 });
}

int AudioManager::PlaySound(std::string sound, int importance, vec3_t position)
{
	waveform_t wav;
	if (waveforms.find(sound) == waveforms.end())
	{
		wav = Waveform::LoadSound(sound);
		waveforms[sound] = wav;
	}
	else
		wav = waveforms[sound];

	int channel = -1;
	int leastimporant = 0;
	for (int i = 0; i < AUDIO_NUMCHANNELS; i++)
	{
		if (channels[i].importance < channels[leastimporant].importance)
			leastimporant = i;

		if (!channels[i].playing)
		{
			channel = i;
			break;
		}
	}

	if (channel == -1)
	{
		if (channels[leastimporant].importance > importance)
			return -1;

		channel = leastimporant;
	}

	if (channels[channel].thread.joinable())
		channels[channel].thread.join();

	if (channels[channel].sound.sound)
		channels[channel].sound.sound.release();

	channels[channel].sound = waveforms[sound];
	channels[channel].importance = importance;
	channels[channel].playing = true;
	channels[channel].position = { 0.0, 0.0, 0.0 };
	channels[channel].pitch = 1.0;
	channels[channel].volume = 1.0;
	channels[channel].timeleft = waveforms[sound].duration;
	
	channels[channel].thread = std::thread(&AudioManager::PlaySoundOnChannel, this, channel, waveforms[sound], position, 1.0, 100.0);

	return channel;
}

void AudioManager::PlaySoundOnChannel(int channel, waveform_t sound, vec3_t position, float pitch, float volume)
{
	
	sf::Sound s;
	s.setBuffer(*sound.sound);
	s.setPitch(pitch);
	s.setVolume(volume);
	Mat3x4 test = Game::GetGame().camera.inv;
	Vector3 posv = test * Vector3(position);
	posv.normalize();
	s.setPosition(sf::Vector3f(posv.get(0), posv.get(1), posv.get(2)));
	s.play();

	while (s.getStatus() == sf::Sound::Playing)
	{
		if (Game::GetGame().IsPaused())
		{
			s.pause();
			while (Game::GetGame().IsPaused())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			s.play();
		}

		Vector3 posv = test * Vector3(position);
		posv.normalize();
		s.setPosition(sf::Vector3f(posv.get(0), posv.get(1), posv.get(2)));
		channels[channel].timeleft -= 0.001F;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	channels[channel].playing = false;
	channels[channel].sound.sound.release();
}