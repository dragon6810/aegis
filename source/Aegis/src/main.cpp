#include <stdio.h>

#include "Game.h"

int main(int argc, char** argv)
{
	int i;

	std::string cmds;

	for (i = 1; i < argc; i++)
		cmds += std::string(argv[i]) + " ";
	
	Game::GetGame().ParseCommands(cmds);
	Game::GetGame().Run();
}