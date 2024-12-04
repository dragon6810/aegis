#pragma once

#include <atomic>

#include "Window.h"
#include "Renderer.h"
#include "World.h"
#include "Console.h"

#define ENGINE_TICKRATE 50
#define ENGINE_TICKDUR_MS (1000 / ENGINE_TICKRATE)
#define ENGINE_TICKDUR (1.0 / (float) ENGINE_TICKRATE)

class Game
{
// https://stackoverflow.com/questions/1008019/how-do-you-implement-the-singleton-design-pattern
public:
	static Game& GetGame(void)
	{
		static Game instance;
		return instance;
	}
private:
	Game(void) {}

	Game(Game const&);
	void operator=(Game const&);

// Real Stuff begins here:

public:
	float intertick;
	float deltatime;
    std::vector<void*> tickprops;

	World world;
	Console console;

	int windoww, windowh;

	void Run(void); // Start the game

	bool ParseCommands(std::string cmd);
private:
	Window window;
	Renderer renderer;

	long long lasttick;

	// Startup
	void AutoExecute(void); // Run userdata/autoexec.cfg on startup (config)

	// Heartbeat
	bool Loop(void);        // Run as fast as possible
	void Tick(void);        // Run ENGINE_TICKRATE times per second
	void Render(void);      // Run as fast as possible OR as fast as vsync allows
};
