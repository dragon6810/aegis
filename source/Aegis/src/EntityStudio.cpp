#include "EntityStudio.h"

#include <c_string.h>

#include "Console.h"
#include "Command.h"

#define VERBOSE_STUDIO_LOGGING 0

void EntityStudio::Init(const std::unordered_map <std::string, std::string>& pairs)
{
    pos = LoadVector3(pairs, "origin", Vector3(0,0,0));
    rot = LoadVector3(pairs, "angles", Vector3(0,0,0));

    LoadModel();
}

std::string EntityStudio::GetModelName()
{
    return "models/agrunt.mdl";
}

EntityStudio::anim_t EntityStudio::LoadAnimation(FILE* ptr, uint32_t offset, int nframes, int nblends)
{
    int f, i, j, k;

    anim_t anim;
    uint64_t before;
    short offsets[6];
    short val;
    uint8_t total, valid;

    before = ftell(ptr);
    
    anim.nframes = nframes;
    anim.data.resize(bones.size());
    for(i=0; i<bones.size(); i++)
    {
        anim.data[i].nframes = nframes;
        anim.data[i].pos.resize(nframes);
        anim.data[i].rot.resize(nframes);
        fseek(ptr, offset + (i + bones.size() * 0) * 12, SEEK_SET);
        fread(offsets, sizeof(short), 6, ptr);
   
        for(j=0; j<3; j++)
        {
            for(f=0; f<nframes; f++)
            {
                anim.data[i].pos[f][j] = bones[i].defpos[j];
                if(!offsets[j])
                    break;            
                
                fseek(ptr, offset + offsets[j], SEEK_SET);
                fread(&val, sizeof(short), 1, ptr);
                total = val & 0xFF;
                valid = (val >> 8) & 0xFF;
                //Console::Print("anim val: %u, %u, %hd\n", total, valid, val);
            }
        }
    }

    fseek(ptr, before, SEEK_SET);

    return anim;
}

EntityStudio::seqdesc_t EntityStudio::LoadSequence(FILE* ptr)
{
    int i;

    seqdesc_t seq;

    char name[33];
    int ibone, ianim;

    fread(name, 1, 32, ptr);
    fread(&seq.fps, sizeof(float), 1, ptr);
    fseek(ptr, sizeof(int), SEEK_CUR);
    fread(&seq.activity, sizeof(int), 1, ptr);
    fread(&seq.weight, sizeof(float), 1, ptr);
    fseek(ptr, sizeof(int) * 2, SEEK_CUR);
    fread(&seq.nframes, sizeof(int), 1, ptr);
    fseek(ptr, sizeof(int) * 3, SEEK_CUR);
    fread(&ibone, sizeof(int), 1, ptr);
    for(i=0; i<3; i++)
        fread(&seq.displacement[i], sizeof(float), 1, ptr);
    fseek(ptr, sizeof(int) * 2, SEEK_CUR);
    fseek(ptr, sizeof(float) * 3 * 2, SEEK_CUR);
    fseek(ptr, sizeof(int), SEEK_CUR);
    fread(&ianim, sizeof(int), 1, ptr);
    fseek(ptr, 28, SEEK_CUR); // Skip over blends for now
    fseek(ptr, sizeof(int), SEEK_CUR);
    fseek(ptr, sizeof(int) * 4, SEEK_CUR); // Skip over transition graph for now

    name[32] = 0;
    seq.name = std::string(name);
    seq.rootbone = &bones[ibone];
    seq.anim = LoadAnimation(ptr, ianim, seq.nframes, 1);

    return seq;
}

void EntityStudio::LoadSequences(FILE* ptr)
{
    int i;
    seqdesc_t *curseq;

    uint32_t lumpsize, lumpoffs;

    fseek(ptr, 164, SEEK_SET);

    fread(&lumpsize, sizeof(uint32_t), 1, ptr);
    fread(&lumpoffs, sizeof(uint32_t), 1, ptr);

    Console::Print("Sequence count: %d\n", lumpsize);

    fseek(ptr, lumpoffs, SEEK_SET);
    sequences.resize(lumpsize);
    for(i=0, curseq=sequences.data(); i<lumpsize; i++, curseq++)
    {

        fseek(ptr, lumpoffs + i * 176, SEEK_SET);

        *curseq = LoadSequence(ptr);
#if VERBOSE_STUDIO_LOGGING
        Console::Print("Sequence %d:\n", i);
        Console::Print("    Name: %s\n", curseq->name.c_str());
        Console::Print("    FPS: %f\n", curseq->fps);
        Console::Print("    Frame Count: %d\n", curseq->nframes);
#endif
    }
}

void EntityStudio::LoadControllers(FILE* ptr)
{
    int i;
    controller_t *curctl;

    uint32_t lumpsize, lumpoffs;

    int ibone;
    int index;

    fseek(ptr, 148, SEEK_SET);

    fread(&lumpsize, sizeof(uint32_t), 1, ptr);
    fread(&lumpoffs, sizeof(uint32_t), 1, ptr);

    Console::Print("Controller count: %d\n", lumpsize);

    fseek(ptr, lumpoffs, SEEK_SET);
    controllers.resize(lumpsize);
    for(i=0, curctl=controllers.data(); i<lumpsize; i++, curctl++)
    {
        fread(&ibone, sizeof(int), 1, ptr);
        fread(&curctl->type, sizeof(int), 1, ptr);
        fread(&curctl->min, sizeof(float), 1, ptr);
        fread(&curctl->max, sizeof(float), 1, ptr);
        fread(&curctl->def, sizeof(float), 1, ptr);
        fread(&index, sizeof(int), 1, ptr);

        curctl->bone = &bones[ibone];
        ctlindices[index] = curctl;
#if VERBOSE_STUDIO_LOGGING
        Console::Print("Controller %d:\n", i);
        Console::Print("    Bone index: %d\n", ibone);
        Console::Print("    Controller index: %d\n", index);
#endif
    }
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
#if VERBOSE_STUDIO_LOGGING
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
    LoadControllers(ptr);
    LoadSequences(ptr);

    fclose(ptr);
}
