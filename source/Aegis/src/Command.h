#pragma once

#include <string>
#include <unordered_map>

class Command
{
public:
	static const std::unordered_map<std::string, bool(*)(std::string)> cmdtable;

	static void Run(std::string key, std::string val);

	static bool CommandMap(std::string val);
};