#pragma once
#include "BaseEntity.h"
class BSPModelEntity : public BaseEntity
{
public:
	BSPModelEntity(BSPMap& map);

	void SetModel(int whichmodel);

	virtual void Render() override;
private:
	int whichmodel = 0;
};

