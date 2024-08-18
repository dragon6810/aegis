//
//  GruntEntity.h
//  Aegis
//
//  Created by Henry Dunn on 8/17/24.
//


#include "MonsterEntity.h"

class GruntEntity : public MonsterEntity
{
public:
    GruntEntity(BSPMap& map);
    
    virtual std::string GetStudioPath() override;
    virtual int GetPathMode() override;
    virtual int GetClippingHull() override;
    virtual float GetMaxSpeed() override;
};
