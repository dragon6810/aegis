#pragma once

#include <unordered_map>
#include <string>

#include "Vector.h"

class EntityBase
{
public:
	virtual ~EntityBase(void) = default;
	virtual void Init(const std::unordered_map <std::string, std::string>& pairs) = 0;
    virtual void Render(void) = 0;
    virtual void Tick(void) = 0;
protected:
	int LoadInt(const std::unordered_map <std::string, std::string>& pairs, std::string key, int _default);
	float LoadFloat(const std::unordered_map <std::string, std::string>& pairs, std::string key, float _default);
	Vector3 LoadVector3(const std::unordered_map <std::string, std::string>& pairs, std::string key, Vector3 _default);
};
