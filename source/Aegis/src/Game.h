#pragma once

#include "Window.h"
#include "Renderer.h"
#include "World.h"

#define ENGINE_TICKRATE 50
#define ENGINE_TICKDUR_MS 1000 / ENGINE_TICKRATE
#define ENGINE_TICKDUR 1f / (float) ENGINE_TICKRATE

class Game
{
// https://stackoverflow.com/questions/1008019/how-do-you-implement-the-singleton-design-pattern
public:
	static Game& GetGame()
	{
		static Game instance;
		return instance;
	}
private:
	Game() {}

	Game(Game const&);
	void operator=(Game const&);

// Real Stuff begins here:

public:
	bool running;
	float intertick;

	void Run(); // Start the game

	bool ParseCommands(std::string cmd);
private:
	Window window;
	Renderer renderer;
	World world;

	long long lasttick;

	bool Loop(); // Run as fast as possible
	void Tick();
	void Render();
};