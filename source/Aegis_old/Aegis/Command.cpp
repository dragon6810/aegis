#include "Command.h"

std::string Command::Execute()
{
	if (!func)
		printf("Command \"%s\" called without giving a corresponding function pointer!", (cmd + " " + val).c_str());

	return func(val);
}