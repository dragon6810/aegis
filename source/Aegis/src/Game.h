#pragma once

#define ENGINE_TICKRATE 50
#define ENGINE_TICKDUR_MS 1000 / ENGINE_TICKRATE
#define ENGINE_TICKDUR 1f / (float) ENGINE_TICKRATE

class Game
{
public:
	float intertick;

	void Run(); // Start the game
private:
	bool Loop(); // Run as fast as possible
	void Tick();
	void Render();

	long long lasttick;
};