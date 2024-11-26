#include "EntityStudio.h"

#include <c_string.h>

#include "Console.h"
#include "Command.h"
#include "Quaternion.h"

#define VERBOSE_STUDIO_LOGGING 1

void EntityStudio::Init(const std::unordered_map <std::string, std::string>& pairs)
{
    pos = LoadVector3(pairs, "origin", Vector3(0,0,0));
    rot = LoadVector3(pairs, "angles", Vector3(0,0,0));

    LoadModel();
}

void EntityStudio::Render(void)
{
    UpdateBones();
    DrawSkeleton();
}

std::string EntityStudio::GetModelName()
{
    return "models/barney";
}

void EntityStudio::UpdateBoneMatrix(bone_t* bone)
{
    Quaternion q;

    q = Quaternion::FromEuler(bone->currot);

    bone->transform = q.ToMatrix4();
    bone->transform[0][3] = bone->curpos[0];
    bone->transform[1][3] = bone->curpos[1];
    bone->transform[2][3] = bone->curpos[2];
}

void EntityStudio::UpdateBones(void)
{
    int i;

    for(i=0; i<bones.size(); i++)
        UpdateBoneMatrix(&bones[i]);

    for(i=0; i<bones.size(); i++)
    {
        if(!bones[i].parent)
            continue;

        bones[i].transform = bones[i].parent->transform * bones[i].transform; 
    }
}

void EntityStudio::DrawSkeleton(void)
{
    Vector3 zero(0, 0, 0);

    int i, j;
    Vector3 root, cur;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[10]->name);
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1); glVertex3f( 128,  128, 0);
    glTexCoord2f(0, 1); glVertex3f(-128,  128, 0);
    glTexCoord2f(0, 0); glVertex3f(-128, -128, 0);
    glTexCoord2f(1, 0); glVertex3f( 128, -128, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glColor3f(1, 0, 0);
    glBegin(GL_LINES);

    for(i=0; i<bones.size(); i++)
    {
        root = bones[i].transform * zero * 5.0f;
        for(j=0; j<bones[i].children.size(); j++)
        {
            cur = bones[i].children[j]->transform * zero * 5.0f;
            glVertex3f(root[0], root[1], root[2]);
            glVertex3f(cur[0], cur[1], cur[2]);
        }
    }

    glEnd();
    glColor3f(1, 1, 1);
}

ResourceManager::texture_t* EntityStudio::LoadTexture(FILE* ptr)
{
    int i;

    ResourceManager::texture_t *tex;

    char name[65];
    int w, h;
    int offs;

    unsigned char c;
    unsigned char r[256];
    unsigned char g[256];
    unsigned char b[256];
    std::vector<int> pdata;

    fread(name, 1, 64, ptr);

    name[64] = 0;

    tex = ResourceManager::FindTexture(GetModelName(), name);
    if(tex)
        return tex;

    fseek(ptr, sizeof(int), SEEK_CUR);
    fread(&w, sizeof(int), 1, ptr);
    fread(&h, sizeof(int), 1, ptr);
    fread(&offs, sizeof(int), 1, ptr);

    fseek(ptr, offs + w * h, SEEK_SET);
    for(i=0; i<256; i++)
    {
        fread(&r[i], 1, 1, ptr);
        fread(&g[i], 1, 1, ptr);
        fread(&b[i], 1, 1, ptr);
    }

    fseek(ptr, offs, SEEK_SET);
    pdata.resize(w * h);
    for(i=0; i<w*h; i++)
    {
        fread(&c, 1, 1, ptr);
        pdata[i] = 0;
        pdata[i] |= ((int) r[c]) <<  0;
        pdata[i] |= ((int) g[c]) <<  8;
        pdata[i] |= ((int) b[c]) << 16;
        if(pdata[i] != 0x00FF0000)
            pdata[i] |= 0xFF000000;
    }

    tex = ResourceManager::NewTexture();
    tex->id = name;
    tex->source = GetModelName();
    tex->width = w;
    tex->height = h;

    glBindTexture(GL_TEXTURE_2D, tex->name);
        
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
    if(Command::filtertextures)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
        
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pdata.data());

    ResourceManager::UseTexture(tex);
    return tex;
}

void EntityStudio::LoadTextures(FILE* ptr)
{
    int i;
    ResourceManager::texture_t **curtex;
   
    FILE* tptr;
    std::string tname; 
    uint32_t lumpsize, lumpoffs;

    fseek(ptr, 180, SEEK_SET);

    fread(&lumpsize, sizeof(uint32_t), 1, ptr);
    fread(&lumpoffs, sizeof(uint32_t), 1, ptr);

    if(!lumpsize)
    {
        Console::Print("Model has no stored textures, looking for external texture model...\n");
        tname = Command::datadir + GetModelName() + "t.mdl";
        tptr = fopen(tname.c_str(), "rb");
        if(!tptr)
        {
            Console::Print("Couldn't find external texture model \"%s\".\n", tname.c_str());
            return;
        }

        Console::Print("Found external texture model \"%s\".\n", tname.c_str());
        LoadTextures(tptr);

        fclose(tptr);
        return;
    }

    Console::Print("Texture count: %d\n", lumpsize);

    fseek(ptr, lumpoffs, SEEK_SET);
    textures.resize(lumpsize);
    for(i=0, curtex=textures.data(); i<lumpsize; i++, curtex++)
    {

        fseek(ptr, lumpoffs + i * 80, SEEK_SET);
        textures[i] = LoadTexture(ptr);

#if VERBOSE_STUDIO_LOGGING
        Console::Print("Texture %d:\n", i);
        Console::Print("    Name: %s\n", textures[i]->id.c_str());
        Console::Print("    Dimensions: %d x %d\n", textures[i]->width, textures[i]->height);
#endif
    }

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

    path = Command::datadir + GetModelName() + ".mdl";
    
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
    LoadTextures(ptr);

    fclose(ptr);
}
