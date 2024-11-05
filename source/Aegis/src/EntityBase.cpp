#include "EntityBase.h"

int EntityBase::LoadInt(const std::unordered_map <std::string, std::string>& pairs, std::string key, int default)
{
	int val;

	if (pairs.find(key) == pairs.end())
		return default;

	if (sscanf(pairs.at(key).c_str(), "%d", &val) != 1)
		return default;

	return val;
}

float EntityBase::LoadFloat(const std::unordered_map <std::string, std::string>& pairs, std::string key, float default)
{
	float val;

	if (pairs.find(key) == pairs.end())
		return default;

	if (sscanf(pairs.at(key).c_str(), "%f", &val) != 1)
		return default;

	return val;
}

Vector3 EntityBase::LoadVector3(const std::unordered_map <std::string, std::string>& pairs, std::string key, Vector3 default)
{
	Vector3 val;

	if (pairs.find(key) == pairs.end())
		return default;

	if (sscanf(pairs.at(key).c_str(), "%f %f %f", &val.x, &val.y, &val.z) != 3)
		return default;

	return val;
}