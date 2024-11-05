#include "EntityCamera.h"

#include "Console.h"

void EntityCamera::Init(const std::unordered_map <std::string, std::string>& pairs)
{
	pos = LoadVector3(pairs, "origin", Vector3(0, 0, 0));
	rot = LoadVector3(pairs, "angles", Vector3(0, 0, 0));

	Console::Print("Camera Position: %s.\n", pos.ToString().c_str());
	Console::Print("Camera Rotation: %s.\n", rot.ToString().c_str());
}