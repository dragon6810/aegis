#pragma once

#include "BaseEntity.h"

#include <string>

#include "SModel.h"

class MonsterEntity : public BaseEntity
{
public:
	virtual void Render() override;
	virtual void Think(float deltatime) override;
	virtual void Init() override;
	MonsterEntity(BSPMap& map);
    virtual std::string GetStudioPath(); // Override this to set the path to your model
protected:
	SModel model;
};

