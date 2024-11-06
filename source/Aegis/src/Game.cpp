#include "Game.h"

#include <stdio.h>
#include <chrono>
#include <vector>
#include <sstream>
#include <iostream>
#include <thread>
#include <regex>
#include <filesystem>

#include "Command.h"

void Game::Render()
{
	renderer.Clear();

	glEnable(GL_DEPTH_TEST);
	world.Render();

	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,320,0,240,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	console.Render();

	renderer.Submit();
}

void Game::Tick()
{
	// TODO: Tick
}

bool Game::Loop()
{
	long long now;
	long long delta;
	
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

	delta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - now;
	deltatime = (float)delta / 1000.0;

	return !window.ShouldClose();
}

bool Game::ParseCommands(std::string cmd)
{
	std::replace(cmd.begin(), cmd.end(), '\n', ';');

	int i;

	std::vector<std::string> commands;
	std::stringstream ss(cmd);
	std::string token;
	std::string key, val;
	std::regex pattern(R"(\s*(\S+)(?:\s+(.+))?\s*)");
	std::smatch matches;

	while (std::getline(ss, token, ';'))
		commands.push_back(token);

	for (i = 0; i < commands.size(); i++)
	{
		if (std::regex_match(commands[i], matches, pattern))
		{
			key = matches[1];
			if (matches.size() > 2 && matches[2].matched)
			{
				val = matches[2];
				while (val[val.size() - 1] <= 32)
					val.pop_back();
			}
			else
				val = "";

			Command::Run(key, val);
		}
	}

	return true;
}


void Game::AutoExecute()
{
	FILE* ptr;

	uint64_t size;
	std::string commands;
	
	ptr = fopen(Command::autoexec.c_str(), "r");
	if (!ptr)
	{
		Console::Print("autoexec file \"%s\" does not exist.\n", Command::autoexec.c_str());
		return;
	}

	fseek(ptr, 0, SEEK_END);
	size = ftell(ptr);
	fseek(ptr, 0, SEEK_SET);

	commands.resize(size);
	fread(commands.data(), 1, size, ptr);

	ParseCommands(commands);

	fclose(ptr);
}

void Game::Run()
{
	renderer.PreWindow();
	window.MakeWindow(800, 600, "Aegis");
	renderer.PostWindow(&window);

	AutoExecute();
	console.Load();
	
	while (Loop());
}