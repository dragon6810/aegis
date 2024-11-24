#include "EntityStudio.h"

#include <c_string.h>

#include "Console.h"
#include "Command.h"

void EntityStudio::Init(const std::unordered_map <std::string, std::string>& pairs)
{
    pos = LoadVector3(pairs, "origin", Vector3(0,0,0));
    rot = LoadVector3(pairs, "angles", Vector3(0,0,0));

    LoadModel();
}

std::string EntityStudio::GetModelName()
{
    return "models/tank.mdl";
}

void EntityStudio::LoadBones(FILE* ptr)
{
    int i, j;
    bone_t *curbone;

    uint32_t lumpsize, lumpoffs;

    char name[33];
    int iparent;

    fseek(ptr, 140, SEEK_SET);

    fread(&lumpsize, sizeof(uint32_t), 1, ptr);
    fread(&lumpoffs, sizeof(uint32_t), 1, ptr);

    Console::Print("Bone count: %d\n", lumpsize);

    fseek(ptr, lumpoffs, SEEK_SET);
    bones.resize(lumpsize);
    for(i=0, curbone=bones.data(); i<lumpsize; i++, curbone++)
    {
        fread(name, 1, 32, ptr); name[32] = 0;
        fread(&iparent, sizeof(int), 1, ptr);
        fseek(ptr, 4 + 24, SEEK_CUR);

        for(j=0; j<3; j++)
            fread(&curbone->defpos[j], sizeof(float), 1, ptr);
        for(j=0; j<3; j++)
            fread(&curbone->defrot[j], sizeof(float), 1, ptr);
        
        for(j=0; j<3; j++)
            fread(&curbone->scalepos[j], sizeof(float), 1, ptr);
        for(j=0; j<3; j++)
            fread(&curbone->scalerot[j], sizeof(float), 1, ptr);
        
        curbone->name = std::string(name);
        if(i) // Bone 0 has no parent
        {
            curbone->parent = &bones[iparent];
            curbone->parent->children.push_back(curbone);
        }
        curbone->curpos = curbone->defpos;
        curbone->currot = curbone->defrot;
#if 0
        Console::Print("Bone %d:\n", i);
        Console::Print("    Name: \"%s\".\n", curbone->name.c_str());
        Console::Print("    Parent: %d.\n", iparent);
#endif
    }
}

void EntityStudio::LoadHeader(FILE* ptr)
{
    fseek(ptr, 0, SEEK_SET);

    fseek(ptr, 76, SEEK_CUR);

    fread(&eyepos.x, sizeof(float), 1, ptr);
    fread(&eyepos.y, sizeof(float), 1, ptr);
    fread(&eyepos.z, sizeof(float), 1, ptr);

    fseek(ptr, 24, SEEK_CUR);

    fread(&bbmin.x, sizeof(float), 1, ptr);
    fread(&bbmin.y, sizeof(float), 1, ptr);
    fread(&bbmin.z, sizeof(float), 1, ptr);

    fread(&bbmax.x, sizeof(float), 1, ptr);
    fread(&bbmax.y, sizeof(float), 1, ptr);
    fread(&bbmax.z, sizeof(float), 1, ptr);
}

void EntityStudio::LoadModel()
{
    FILE* ptr;
    std::string path;

    char id[5];
    int version;

    path = Command::datadir + GetModelName();
    
    ptr = fopen(path.c_str(), "rb");
    if(!ptr)
    {
        Console::Print("Can not find studio path \"%s\".\n", path.c_str());
        return;
    }

    fread(id, 1, 4, ptr);
    id[4] = 0;
    if(strcmp(id, "IDST"))
    {
        Console::Print("Invalid model file \"%s\".\n", path.c_str());
        fclose(ptr);
        return;
    }
    
    fread(&version, sizeof(int), 1, ptr);
    if(version != 10)
    {
        Console::Print("Wrong model version %d in files \"%s\".\n", version, path.c_str());
        fclose(ptr);
        return;
    }

    LoadHeader(ptr);
    LoadBones(ptr);

    fclose(ptr);
}
