#include "Game.h"

#include <stdio.h>
#include <chrono>
#include <vector>
#include <sstream>
#include <iostream>
#include <thread>
#include <regex>

#ifdef _WIN32
#include <conio.h>       // For Windows input handling
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>  // For POSIX input handling
#endif

#include "Command.h"

bool Game::ParseCommands(std::string cmd)
{
	int i, k;

	std::vector<std::string> commands;
	std::stringstream ss(cmd);
	std::string token;
	std::string key, val;
	std::regex pattern(R"(\s*(\S+)\s+(.+)\s*)");
	std::smatch matches;
	
	while (std::getline(ss, token, ';'))
		commands.push_back(token);

	for (i = 0; i < commands.size(); i++)
	{
		if (std::regex_match(commands[i], matches, pattern)) 
		{
			key = matches[1];
			val = matches[2];
			while (val[val.size() - 1] <= 32)
				val.pop_back();

			Command::Run(key, val);
		}
	}

	return true;
}

void Game::Render()
{
	renderer.Clear();
	renderer.Submit();

	
}

void Game::Tick()
{
	
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

void inputlistener()
{
	std::string input;

	while (Game::GetGame().running)
	{
		std::getline(std::cin, input);  // Blocking call, but it runs in a separate thread
		if (!input.empty())
			Game::GetGame().ParseCommands(input);  // Parse command when input is available
	}
}

void Game::Run()
{
	std::thread inputhread(inputlistener);

	renderer.PreWindow();
	window.MakeWindow(800, 600, "Aegis");
	renderer.PostWindow(&window);
	
	while (Loop());
	running = false;

	inputhread.join();
}