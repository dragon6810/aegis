#pragma once

#include <atomic>

#include "Window.h"
#include "Renderer.h"
#include "World.h"
#include "Console.h"

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
	float intertick;
	float deltatime;

	World world;
	Console console;

	void Run(); // Start the game

	bool ParseCommands(std::string cmd);
private:
	Window window;
	Renderer renderer;

	long long lasttick;

	// Startup
	void AutoExecute(); // Run userdata/autoexec.cfg on startup (config)

	// Heartbeat
	bool Loop();        // Run as fast as possible
	void Tick();        // Run ENGINE_TICKRATE times per second
	void Render();      // Run as fast as possible OR as fast as vsync allows
};