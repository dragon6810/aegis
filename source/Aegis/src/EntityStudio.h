#pragma once

#include "EntityBase.h"

class EntityStudio : public EntityBase
{
public:
    void Init(const std::unordered_map <std::string, std::string>& pairs) override;

    std::string GetModelName();

    Vector3 pos;
    Vector3 rot;

protected:
    typedef struct bone_t bone_t;

    Vector3 eyepos;
    Vector3 bbmin, bbmax;

    std::vector<bone_t> bones;

private:
    void LoadModel();

    void LoadHeader(FILE* ptr);
    void LoadBones(FILE* ptr);

protected:
    struct bone_t
    {
        std::string name;
        bone_t* parent;
        std::vector<bone_t*> children;

        Vector3 defpos;
        Vector3 defrot;
        Vector3 scalepos;
        Vector3 scalerot;
        Vector3 curpos;
        Vector3 currot;
    };
};