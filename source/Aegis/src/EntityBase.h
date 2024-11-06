#pragma once

#include <unordered_map>
#include <string>

#include "Vector.h"

class EntityBase
{
public:
	virtual ~EntityBase() = default;
	virtual void Init(const std::unordered_map <std::string, std::string>& pairs) = 0;
protected:
	int LoadInt(const std::unordered_map <std::string, std::string>& pairs, std::string key, int _default);
	float LoadFloat(const std::unordered_map <std::string, std::string>& pairs, std::string key, float _default);
	Vector3 LoadVector3(const std::unordered_map <std::string, std::string>& pairs, std::string key, Vector3 _default);
};