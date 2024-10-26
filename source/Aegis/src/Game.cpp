#include "Game.h"

#include <stdio.h>
#include <chrono>

void Game::Render()
{
	renderer.Clear();
	renderer.Submit();

	printf("Render\n");
}

void Game::Tick()
{
	printf("Tick\n");
}

bool Game::Loop()
{
	long long now;
	
	// It should be illegal for it to be this hard to get the time
	now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	if (!lasttick)
	{
		lasttick = now;
		Tick();
	}

	while (now - lasttick >= ENGINE_TICKDUR_MS)
	{
		lasttick += ENGINE_TICKDUR_MS;
		Tick();
	}

	Render();

	return !window.ShouldClose();
}

void Game::Run()
{
	renderer.PreWindow();
	window.MakeWindow(800, 600, "Aegis");
	renderer.PostWindow(&window);

	while (Loop());
}