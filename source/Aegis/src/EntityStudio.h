#pragma once

#include "EntityBase.h"

#include <vector>
//include <unordered_map>

#include "Matrix.h"
#include "TickProperty.h"

class EntityStudio : public EntityBase
{
public:
    void Init(const std::unordered_map <std::string, std::string>& pairs) override;

    std::string GetModelName();

    Vector3 pos;
    Vector3 rot;

protected:
    typedef struct bone_s
    {
        std::string name;
        struct bone_s* parent;
        std::vector<struct bone_s*> children;

        Vector3 defpos;
        Vector3 defrot;
        Vector3 scalepos;
        Vector3 scalerot;
        Vector3 curpos;
        Vector3 currot;

        Matrix4x4 transform;
    } bone_t;

    typedef enum
    {
        MOTION_X,
        MOTION_Y,
        MOTION_Z,
        MOTION_XR,
        MOTION_YR,
        MOTION_ZR,
    } motiontype_e;

    typedef struct controller_s
    {
        bone_t* bone;
        motiontype_e type;
        float min, max, def;
        float cur;
    } controller_t;

protected:
    Vector3 eyepos;
    Vector3 bbmin, bbmax;

    std::vector<bone_t> bones;
    std::unordered_map<int, controller_t*> controllerindices;
    std::vector<controller_t> controllers;
private:
    void LoadModel();

    void LoadHeader(FILE* ptr);
    void LoadBones(FILE* ptr);
    void LoadControllers(FILE* ptr);
};
