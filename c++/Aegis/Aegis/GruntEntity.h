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
};
