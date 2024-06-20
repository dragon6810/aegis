#include "Game.h"

#include <chrono>

void Game::Init()
{
	start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

float Game::Time()
{
	long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	float delta = (float)(now - start) / 1000.0;
	return delta;
}

float Game::R_Random(float min, float max)
{
	r_seed = (r_seed * 1664525 + 1013904223) & 0xFFFFFFFF;

	float normalized = static_cast<float>(r_seed) / 0xFFFFFFFF;
	return min + normalized * (max - min);
}