#pragma once

#include <unordered_map>
#include <string>

#include <Eigen/Dense>

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
	Eigen::Vector3f LoadVector3(const std::unordered_map <std::string, std::string>& pairs, std::string key, Eigen::Vector3f _default);
	Eigen::Vector3f LoadEuler(const std::unordered_map <std::string, std::string>& pairs, std::string key, Eigen::Vector3f _default);
};
