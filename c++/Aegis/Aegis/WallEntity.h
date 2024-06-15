#pragma once

#include "BSPModelEntity.h"

class WallEntity : public BSPModelEntity
{
public:
    WallEntity(BSPMap& map);

    virtual void Render() override;
};

