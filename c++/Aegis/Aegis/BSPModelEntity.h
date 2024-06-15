#pragma once
#include "BaseEntity.h"
class BSPModelEntity : public BaseEntity
{
public:
	BSPModelEntity(BSPMap& map);

	void SetModel(int whichmodel);

	virtual void Init() override;
	virtual void Render() override;
protected:
	int whichmodel = 0;
};

