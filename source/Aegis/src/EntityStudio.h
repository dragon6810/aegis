#pragma once

#include "EntityBase.h"

#include <vector>

#include "Matrix.h"
#include "TickProperty.h"
#include "ResourceManager.h"
#include "Quaternion.h"

class EntityStudio : public EntityBase
{
public:
    void Init(const std::unordered_map <std::string, std::string>& pairs) override;
    void Render(void) override;
    void Tick(void) override;

    std::string GetModelName(void);

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
        Quaternion currot;

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

    // Decompressed animation for 1 bone
    typedef struct boneanim_s
    {
        int nframes;
        bone_t* bone;
        std::vector<Vector3> pos;
        std::vector<Vector3> rot;
    } boneanim_t;

    typedef struct anim_s
    {
        int nframes;
        std::vector<boneanim_t> data;
    } anim_t;

    typedef struct seqdesc_s
    {
        std::string name;
        float fps;
        int nframes;
        int activity;
        float weight;

        // TODO: Events

        bone_t* rootbone;
        Vector3 displacement; // Displacement of rootbone

        // TODO: Blends

        anim_t anim;
    } seqdesc_t;

    typedef enum
    {
        MESH_STRIP,
        MESH_FAN,
    } meshtype_e;

    typedef struct mesh_s
    {
        meshtype_e type;
        std::vector<Vector3> verts;
        std::vector<Vector3> normals;
        std::vector<Vector2> coords;
        std::vector<bone_t*> bones;
        ResourceManager::texture_t* tex;
    } mesh_t;

    typedef struct model_s
    {
        std::string name;

        std::vector<mesh_t> meshes;
    } model_t;

protected:
    Vector3 eyepos;
    Vector3 bbmin, bbmax;
    int curseq = 0;
    TickProperty<float> frame;

    std::vector<bone_t> bones;
    std::unordered_map<int, controller_t*> ctlindices;
    std::vector<controller_t> controllers;
    std::vector<seqdesc_t> sequences;
    std::vector<ResourceManager::texture_t*> textures;
    std::vector<model_t> models;
private:
    void LoadModel(void);
    std::vector<FILE*> GetSeqFiles(FILE* ptr);

    void LoadHeader(FILE* ptr);
    void LoadBones(FILE* ptr);
    void LoadControllers(FILE* ptr);
    void LoadSequences(FILE* ptr, std::vector<FILE*> ptrs);
    void LoadTextures(FILE* ptr);
    void LoadBodyParts(FILE* ptr, int body);

    seqdesc_t LoadSequence(FILE* ptr, std::vector<FILE*> ptrs);
    anim_t LoadAnimation(FILE* ptr, uint32_t offset, int nframes, int nblends);
    ResourceManager::texture_t* LoadTexture(FILE* ptr);
    model_t LoadModel(FILE* ptr);

    void UpdateBones(void);
    void UpdateBoneMatrix(bone_t* bone);
    void DrawSkeleton(void);
    void DrawModel(void);
    void DrawMesh(mesh_t* m);
};
