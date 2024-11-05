#pragma once

#include <unordered_map>
#include <string>

class EntityBase
{
public:
	virtual ~EntityBase() = default;
	virtual void Init(const std::unordered_map <std::string, std::string>& pairs) = 0;
};