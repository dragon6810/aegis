#pragma once

#include <string>

class Command
{
public:
	std::string cmd;
	std::string val;

	std::string(*func)(std::string);

	std::string Execute();
};

