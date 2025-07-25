#include "EntityBase.h"

int EntityBase::LoadInt(const std::unordered_map <std::string, std::string>& pairs, std::string key, int _default)
{
	int val;

	if (pairs.find(key) == pairs.end())
		return _default;

	if (sscanf(pairs.at(key).c_str(), "%d", &val) != 1)
		return _default;

	return val;
}

float EntityBase::LoadFloat(const std::unordered_map <std::string, std::string>& pairs, std::string key, float _default)
{
	float val;

	if (pairs.find(key) == pairs.end())
		return _default;

	if (sscanf(pairs.at(key).c_str(), "%f", &val) != 1)
		return _default;

	return val;
}

Eigen::Vector3f EntityBase::LoadVector3(const std::unordered_map <std::string, std::string>& pairs, std::string key, Eigen::Vector3f _default)
{
	Eigen::Vector3f val;

	if (pairs.find(key) == pairs.end())
		return _default;

	if (sscanf(pairs.at(key).c_str(), "%f %f %f", &val[0], &val[1], &val[2]) != 3)
		return _default;

	return val;
}

Eigen::Vector3f EntityBase::LoadEuler(const std::unordered_map <std::string, std::string>& pairs, std::string key, Eigen::Vector3f _default)
{
	Eigen::Vector3f ent, game;

	if (pairs.find(key) == pairs.end())
		return _default;

	ent = LoadVector3(pairs, key, Eigen::Vector3f());
	game[0] = ent[2];
	game[1] = -ent[0];
	game[2] = ent[1];
}