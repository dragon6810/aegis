#pragma once

#include "BaseEntity.h"

#include <string>

#include "SModel.h"

#define PATH_NONE -1 // No pathfinding (e.g. tank)
#define PATH_DOOM 0  // Doom pathfinding (e.g. grunt)
#define PATH_GRAPH 1 // Smart pathfinding (e.g. spitter)

#define ATTENTION_SPAN 64

class MonsterEntity : public BaseEntity
{
public:
    vec3_t vel{};
    
	virtual void Render() override;
	virtual void Think(float deltatime) override;
	virtual void Init() override;
	MonsterEntity(BSPMap& map);
    virtual std::string GetStudioPath(); // Override this to set the path to your model
    virtual int GetPathMode(); // Override this to set the pathfinding mode
    virtual int GetClippingHull(); // Override this to set the size of world collision
    virtual float GetMaxSpeed(); // Ovveride this to set the maximum speed of the monster
protected:
	SModel model;
    
    bspplane_t floorplane, hitplane;
    vec3_t target, dest; // Target is long term target, dest is short term
    int attention;
    float idealyaw, realyaw;
    int nlocks;
    
    void Gravity();
    void FindFloor();
    void Turn();
    void LockOn();
    void Advance();
    void CantAdvance();

    bool CanAdvance(vec3_t p, vec3_t v);
    bool Sweep(vec3_t p, vec3_t v, vec3_t* result);
    vec3_t Slide(vec3_t p, vec3_t v, vec3_t n);
};
