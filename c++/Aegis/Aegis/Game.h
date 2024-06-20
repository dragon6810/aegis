#pragma once

#define ENGINE_TICKRATE 50
#define ENGINE_TICKDUR 0.02

#define SCREEN_HIGH_WIDTH 1152
#define SCREEN_HIGH_HEIGHT 864

#define SCREEN_MED_WIDTH 640
#define SCREEN_MED_HEIGHT 480

#define SCREEN_LOW_WIDTH 320
#define SCREEN_LOW_HEIGHT 240

#include <GL/glew.h>
#include "Renderer.h"
#include "Window.h"

#include "Wad.h"
#include "BSPMap.h"

class Game
{
public:
	static Game& GetGame()
	{
		static Game instance;
		return instance;
	}

	// Heartbeat
	void Main();
	void Tick();
	void Render();

	float tickinterp = 0.0; // The interpolation factor for each dick

	// Heartbeat getters
	Renderer* GetRenderer();

	// Utils
	float Time(); // Time, in seconds since start of program
	float R_Random(float min, float max); // Use for things like rendering, client based stuff
private:
	Game() {}

	Game(Game const&);
	void operator=(Game const&);

	long long start;

	unsigned int r_seed = 1993;

	Renderer renderer;
	Window* window;

	Wad wad;
	BSPMap map;
};

