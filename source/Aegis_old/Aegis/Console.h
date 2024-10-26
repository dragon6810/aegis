#pragma once

#include <string>

#include "Command.h"

#define NCOMMANDS 2

class Console
{
public:
	const std::string commandmap[NCOMMANDS] =
	{
		"map",
	};

	std::string (*commands[NCOMMANDS])(std::string) =
	{
		Console::CommandMap,
		nullptr,
	};

	std::string Execute(std::string cmd);

	static std::string CommandMap(std::string val); // Unrelated to the commandmap array
private:
	Command cmd;
};

