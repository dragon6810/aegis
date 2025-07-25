#include "EntityStudio.h"

#include <string.h>

#include <mathlib.h>

#include "Console.h"
#include "Command.h"

#define VERBOSE_STUDIO_LOGGING 1

bool EntityStudio::drawstudio = true;
bool EntityStudio::drawskeleton = false;

void EntityStudio::Init(const std::unordered_map <std::string, std::string>& pairs)
{
    pos = LoadVector3(pairs, "origin", Eigen::Vector3f(0,0,0));
    rot = LoadVector3(pairs, "angles", Eigen::Vector3f(0,0,0));

    LoadModel();
}

void EntityStudio::Render(void)
{
    this->transform = Eigen::Matrix4f::Identity();
    this->transform.topLeftCorner<3, 3>() = Mathlib::FromEuler(this->rot).toRotationMatrix();
    this->transform.topRightCorner<3, 1>() = this->pos;

    if(sequences.size())
    {
        while(frame >= sequences[curseq].nframes)
            frame.snap(frame - sequences[curseq].nframes);
        
        UpdateBones();
    }
    if(drawskeleton)
        DrawSkeleton();
    if(drawstudio)
        DrawModel();
}

void EntityStudio::Tick(void)
{
    int i;
    
    for(i=0; i<controllers.size(); i++)
    {
        controllers[i].lastlastrot = controllers[i].lastrot;
        controllers[i].lastrot = controllers[i].rot;
    }

    if(sequences.size())
        frame += ((float) sequences[curseq].fps) / ((float) ENGINE_TICKRATE);
}

std::string EntityStudio::GetModelName(void)
{
    return "models/agrunt";
}

void EntityStudio::UpdateBoneMatrix(bone_t* bone)
{
    int i, j;

    int f, n;
    float t;
    Eigen::Vector3f curpos, nextpos, addpos, realpos;
    Eigen::Quaternionf currot, nextrot, addrot, realrot;

    f = (int) frame;
    n = (f + 1) % sequences[curseq].nframes;
    t = frame - f;

    curpos  = sequences[curseq].anim.data[bone - bones.data()].pos[f];
    nextpos = sequences[curseq].anim.data[bone - bones.data()].pos[n];

    currot  = Mathlib::FromEuler(sequences[curseq].anim.data[bone - bones.data()].rot[f]);
    nextrot = Mathlib::FromEuler(sequences[curseq].anim.data[bone - bones.data()].rot[n]);

    realpos = sequences[curseq].anim.data[bone - bones.data()].pos[f];
    
    realrot = currot.slerp(t, nextrot);

    if(bone->controller)
    {
        switch(bone->controller->type & 0x7FFF)
        {
            case MOTION_X:
                addpos[0] = bone->controller->cur;
                break;
            case MOTION_Y:
                addpos[1] = bone->controller->cur;
                break;
            case MOTION_Z:
                addpos[2] = bone->controller->cur;
                break;
            case MOTION_XR:
                addrot = Mathlib::FromEuler(DEG2RAD(bone->controller->cur) * Eigen::Vector3f::UnitX());
                break;
            case MOTION_YR:
                addrot = Mathlib::FromEuler(DEG2RAD(bone->controller->cur) * Eigen::Vector3f::UnitY());
                break;
            case MOTION_ZR:
                addrot = Mathlib::FromEuler(DEG2RAD(bone->controller->cur) * Eigen::Vector3f::UnitZ());
                break;
            default:
                break;
        }
        bone->controller->pos = addpos;
        bone->controller->rot = addrot;
        addrot = bone->controller->lastlastrot.slerp(Game::GetGame().intertick, bone->controller->lastrot);
    }

    bone->curpos = sequences[curseq].anim.data[bone - bones.data()].pos[f] + addpos;
    bone->currot = currot.slerp(t, nextrot) * addrot;
    
    bone->transform = Eigen::Matrix4f::Identity();
    bone->transform.topLeftCorner<3, 3>() = bone->currot.toRotationMatrix();
    bone->transform.topRightCorner<3, 1>() = bone->curpos;

    bone->noctl = Eigen::Matrix4f::Identity();
    bone->noctl.topLeftCorner<3, 3>() = realrot.toRotationMatrix();
    bone->noctl.topRightCorner<3, 1>() = realpos;
}

void EntityStudio::UpdateBones(void)
{
    int i;

    for(i=0; i<bones.size(); i++)
    {
        UpdateBoneMatrix(&bones[i]);
        if(!bones[i].parent)
            continue;

        bones[i].transform = bones[i].parent->transform * bones[i].transform;
        bones[i].noctl = bones[i].parent->noctl * bones[i].noctl;
    }
}

void EntityStudio::DrawSkeleton(void)
{
    int i, j;
    Eigen::Vector3f root, cur;

    glColor3f(1, 0, 0);
    glBegin(GL_LINES);

    for(i=0; i<bones.size(); i++)
    {
        root = bones[i].transform.topRightCorner<3, 1>();
        for(j=0; j<bones[i].children.size(); j++)
        {
            cur = bones[i].children[j]->transform.topRightCorner<3, 1>();
            glVertex3f(root[0], root[1], root[2]);
            glVertex3f(cur[0], cur[1], cur[2]);
        }
    }

    glEnd();
    glColor3f(1, 1, 1);
}

void EntityStudio::DrawMesh(mesh_t* m)
{
    int i;

    Eigen::Vector3f p, n;

    // Model is wound clockwise
    // In the future patch the winding during load time
    glDisable(GL_CULL_FACE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m->tex->name);

    if(m->type == MESH_FAN)
        glBegin(GL_TRIANGLE_FAN);
    else if(m->type == MESH_STRIP)
        glBegin(GL_TRIANGLE_STRIP);
    else
    {
        Console::Print("Error: unknown mesh type %d.\n", m->type);
        glDisable(GL_TEXTURE_2D);
        return;
    }

    for(i=0; i<m->verts.size(); i++)
    {
        p = m->verts[i];
        n = m->normals[i];

        p = (this->transform * m->bones[i]->transform * TOHOMOGENOUS(p)).head<3>();
        n = (m->bones[i]->transform * TOHOMOGENOUSZEXT(n)).head<3>();

        glTexCoord2f(m->coords[i][0], m->coords[i][1]);
        glVertex3f(p[0], p[1], p[2]);
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void EntityStudio::DrawModel(void)
{
    int m, i;

    for(m=0; m<models.size(); m++)
    {
        for(i=0; i<models[m].meshes.size(); i++)
            DrawMesh(&models[m].meshes[i]);
    }
}

// This function is a mess and I don't like it
// In the future could we make LoadMesh it's own function?
// I don't know why I say we I'm the only programmer :(
// I might make a style guide soon just to force myself to write better code...
// I don't think a function deserves to be more than 80 lines or so
EntityStudio::model_t EntityStudio::LoadModel(FILE* ptr)
{   
    int i, j, k;
    mesh_t *curmesh;

    model_t mdl;

    char name[65];
    int nmeshes, imesh;
    int nverts, iverts, ivertinfo;
    int nnorms, inorms, inorminfo;

    std::vector<uint8_t> vertinfo;
    std::vector<uint8_t> norminfo;
    std::vector<Eigen::Vector3f> verts;
    std::vector<Eigen::Vector3f> norms;

    int itriverts, nsequences;
    int itex;
    short ntriverts, ivert, inorm, s, t;

    fread(name, 1, 64, ptr);
    fseek(ptr, sizeof(int) + sizeof(float), SEEK_CUR);

    fread(&nmeshes, sizeof(int), 1, ptr);
    fread(&imesh, sizeof(int), 1, ptr);

    fread(&nverts, sizeof(int), 1, ptr);
    fread(&ivertinfo, sizeof(int), 1, ptr);
    fread(&iverts, sizeof(int), 1, ptr);

    fread(&nnorms, sizeof(int), 1, ptr);
    fread(&inorminfo, sizeof(int), 1, ptr);
    fread(&inorms, sizeof(int), 1, ptr);

    vertinfo.resize(nverts);
    fseek(ptr, ivertinfo, SEEK_SET);
    fread(vertinfo.data(), sizeof(uint8_t), nverts, ptr);

    norminfo.resize(nnorms);
    fseek(ptr, inorminfo, SEEK_SET);
    fread(norminfo.data(), sizeof(uint8_t), nnorms, ptr);

    verts.resize(nverts);
    fseek(ptr, iverts, SEEK_SET);
    for(i=0; i<nverts; i++)
    {
        for(j=0; j<3; j++)
            fread(&verts[i][j], sizeof(float), 1, ptr);
    }

    norms.resize(nnorms);
    fseek(ptr, inorms, SEEK_SET);
    for(i=0; i<nnorms; i++)
    {
        for(j=0; j<3; j++)
            fread(&norms[i][j], sizeof(float), 1, ptr);
    }

    for(i=0; i<nmeshes; i++)
    {
        fseek(ptr, imesh + i * 20, SEEK_SET);
        fread(&nsequences, sizeof(int), 1, ptr);
        fread(&itriverts, sizeof(int), 1, ptr);
        fread(&itex, sizeof(int), 1, ptr);

        fseek(ptr, itriverts, SEEK_SET);
        do 
        {
            mdl.meshes.push_back({});
            curmesh = &mdl.meshes[mdl.meshes.size() - 1];

            fread(&ntriverts, sizeof(short), 1, ptr);
            curmesh->type = MESH_STRIP;
            if(ntriverts < 0)
            {
                curmesh->type = MESH_FAN;
                ntriverts = -ntriverts;
            }
            
            curmesh->tex = textures[itex];
            curmesh->verts.resize(ntriverts);
            curmesh->normals.resize(ntriverts);
            curmesh->coords.resize(ntriverts);
            curmesh->bones.resize(ntriverts);
            for(k=0; k<ntriverts; k++)
            {
                fread(&ivert, sizeof(short), 1, ptr);
                fread(&inorm, sizeof(short), 1, ptr);
                fread(&s, sizeof(short), 1, ptr);
                fread(&t, sizeof(short), 1, ptr);

                curmesh->bones[k] = &bones[vertinfo[ivert]];
                curmesh->verts[k] = verts[ivert];
                curmesh->normals[k] = norms[inorm];
                curmesh->coords[k][0] = ((float) s) / ((float) curmesh->tex->width);
                curmesh->coords[k][1] = ((float) t) / ((float) curmesh->tex->height);
            }
        }
        while(ntriverts);
    }

    name[64] = 0;
    mdl.name = name;

#if VERBOSE_STUDIO_LOGGING
    Console::Print("    Model:\n");
    Console::Print("        Name: \"%s\".\n", mdl.name.c_str());
    Console::Print("        Number of meshes: %d.\n", mdl.meshes.size());
    Console::Print("        Number of vertices: %d.\n", nverts);
#endif

    return mdl;
}

void EntityStudio::LoadBodyParts(FILE* ptr, int body)
{
    int i;
   
    uint32_t lumpsize, lumpoffs;

    char name[65];
    int nmodels;
    int base;
    int offs;
    int index;

    fseek(ptr, 204, SEEK_SET);

    fread(&lumpsize, sizeof(uint32_t), 1, ptr);
    fread(&lumpoffs, sizeof(uint32_t), 1, ptr);

    Console::Print("Body part count: %d\n", lumpsize);

    models.resize(lumpsize);
    for(i=0; i<lumpsize; i++)
    {
        fseek(ptr, lumpoffs + i * 80, SEEK_SET);

        fread(name, 1, 64, ptr);
        fread(&nmodels, sizeof(int), 1, ptr);
        fread(&base, sizeof(int), 1, ptr);
        fread(&offs, sizeof(int), 1, ptr);

        index = body / base;
        index = index % nmodels;

        fseek(ptr, offs + index * 112, SEEK_SET);

#if VERBOSE_STUDIO_LOGGING
        name[64] = 0;
        Console::Print("Body Part %d:\n", i);
        Console::Print("    Name: \"%s\".\n", name);
        Console::Print("    Number of models: %d.\n", nmodels);
#endif

        models[i] = LoadModel(ptr);
    }
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
    short offsets[6];
    short val;
    uint8_t total, valid;

    anim.nframes = nframes;
    anim.data.resize(bones.size());
    for(i=0; i<bones.size(); i++, offset += 12)
    {
        anim.data[i].nframes = nframes;
        anim.data[i].pos.resize(nframes);
        anim.data[i].rot.resize(nframes);
        fseek(ptr, offset, SEEK_SET);
        fread(offsets, sizeof(short), 6, ptr);

        for(f=0; f<nframes; f++)
        {  
            // Load positions 
            for(j=0; j<3; j++)
            {
                anim.data[i].pos[f][j] = bones[i].defpos[j];
                if(!offsets[j])
                    continue;            
                
                // Find the current span we're in and set k to our
                // location within the span    
                k = f;
                total = valid = 0;
                fseek(ptr, offset + offsets[j], SEEK_SET);
                while(k >= total)
                {
                    k -= total;
                    fseek(ptr, valid * sizeof(short), SEEK_CUR);
                    fread(&valid, 1, 1, ptr);
                    fread(&total, 1, 1, ptr);
                }

                // This frame is run length encoded, use the last valid value.
                if(k >= valid)
                    k = valid - 1;
                
                fseek(ptr, k * sizeof(short), SEEK_CUR);
                fread(&val, sizeof(short), 1, ptr);

                anim.data[i].pos[f][j] += ((float) val) * bones[i].scalepos[j];
            }

            // Load rotations
            for(j=0; j<3; j++)
            {
                anim.data[i].rot[f][j] = bones[i].defrot[j];
                if(!offsets[j+3])
                    continue;            
                
                // Find the current span we're in and set k to our
                // location within the span    
                k = f;
                total = valid = 0; 
                fseek(ptr, offset + offsets[j+3], SEEK_SET);
                while(k >= total)
                {
                    k -= total;
                    fseek(ptr, valid * sizeof(short), SEEK_CUR);
                    fread(&valid, 1, 1, ptr);
                    fread(&total, 1, 1, ptr);
                }

                // This frame is run length encoded, use the last valid value.
                if(k >= valid)
                    k = valid - 1;
                
                if(k < valid)
                {
                    fseek(ptr, k * sizeof(short), SEEK_CUR);
                    fread(&val, sizeof(short), 1, ptr);

                    anim.data[i].rot[f][j] += ((float) val) * bones[i].scalerot[j];
                    continue;
                }
            }
        }
    }

    return anim;
}

EntityStudio::seqdesc_t EntityStudio::LoadSequence(FILE* ptr, std::vector<FILE*> ptrs)
{
    int i;

    seqdesc_t seq;

    char name[33];
    int ibone, ianim, igroup;

    FILE* panim;

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
    fread(&igroup, sizeof(int), 1, ptr);
    fseek(ptr, sizeof(int) * 4, SEEK_CUR); // Skip over transition graph for now

    name[32] = 0;
    seq.name = std::string(name);
    seq.rootbone = &bones[ibone];
    seq.anim = LoadAnimation(ptrs[igroup], ianim, seq.nframes, 1);

    return seq;
}

void EntityStudio::LoadSequences(FILE* ptr, std::vector<FILE*> ptrs)
{
    int i;
    seqdesc_t *curseq;

    uint32_t lumpsize, lumpoffs;

    fseek(ptr, 164, SEEK_SET);

    fread(&lumpsize, sizeof(uint32_t), 1, ptr);
    fread(&lumpoffs, sizeof(uint32_t), 1, ptr);

    Console::Print("Sequence count: %d\n", lumpsize);

    fseek(ptr, lumpoffs, SEEK_SET);
    for(i=0; i<lumpsize; i++)
    {
        sequences.push_back({});
        curseq = &sequences[sequences.size() - 1];

        fseek(ptr, lumpoffs + i * 176, SEEK_SET);

        *curseq = LoadSequence(ptr, ptrs);
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
        curctl->bone->controller = (EntityStudio::controller_s*) curctl;
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
        curbone->currot = Mathlib::FromEuler(curbone->defrot);
#if VERBOSE_STUDIO_LOGGING
        Console::Print("Bone %d:\n", i);
        Console::Print("    Name: \"%s\".\n", curbone->name.c_str());
        Console::Print("    Parent: %d.\n", iparent);
#endif
    }
}

void EntityStudio::LoadHeader(FILE* ptr)
{
    fseek(ptr, 76, SEEK_SET);

    fread(&eyepos[0], sizeof(float), 1, ptr);
    fread(&eyepos[1], sizeof(float), 1, ptr);
    fread(&eyepos[2], sizeof(float), 1, ptr);

    fseek(ptr, 24, SEEK_CUR);

    fread(&bbmin[0], sizeof(float), 1, ptr);
    fread(&bbmin[1], sizeof(float), 1, ptr);
    fread(&bbmin[2], sizeof(float), 1, ptr);

    fread(&bbmax[0], sizeof(float), 1, ptr);
    fread(&bbmax[1], sizeof(float), 1, ptr);
    fread(&bbmax[2], sizeof(float), 1, ptr);

#ifdef VERBOSE_STUDIO_LOGGING
    Console::Print("Model header:\n");
    Console::Print("    Eye position: ( %f %f %f ).\n", eyepos[0], eyepos[1], eyepos[2]);
#endif
}

std::vector<FILE*> EntityStudio::GetSeqFiles(FILE* ptr)
{
    int i;

    int lumpsize;
    std::vector<FILE*> seqfiles;
    std::string name;
    FILE* cur;

    fseek(ptr, 172, SEEK_SET);
    fread(&lumpsize, sizeof(int), 1, ptr);

    Console::Print("%d Sequence groups.\n", lumpsize);
   
    if(!lumpsize) 
        return {};

    seqfiles.push_back(ptr);
    if(lumpsize == 1)
    {
        Console::Print("All sequences stored locally.\n");
        return seqfiles;
    }

    if(lumpsize > 100)
    {
        Console::Print("Error in model: maximum of 100 sequence groups allowed.\n");
        return {};
    }

    Console::Print("Sequences stored externally, attempting to locate sequence files.\n");
    for(i=1; i<lumpsize; i++)
    {
        name = Command::datadir + GetModelName();
        name.push_back('0' + i / 10);
        name.push_back('0' + i % 10);
        name.append(".mdl");
        Console::Print("Loading sequence file \"%s\".\n", name.c_str());
    
        cur = fopen(name.c_str(), "rb");
        if(!cur)
        {
            Console::Print("Warning: failed to find sequence file \"%s\".\n", name.c_str());
            continue;
        }
        seqfiles.push_back(cur);
    }

    return seqfiles;
}

void EntityStudio::LoadModel()
{
    int i;

    FILE* ptr;
    std::vector<FILE*> seqfiles;
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

    seqfiles = GetSeqFiles(ptr);
    
    LoadHeader(ptr);
    LoadBones(ptr);
    LoadControllers(ptr);
    LoadSequences(ptr, seqfiles);
    LoadTextures(ptr);
    LoadBodyParts(ptr, 0);

    for(i=0; i<seqfiles.size(); i++)
    {
        if(seqfiles[i] == ptr)
            continue;

        fclose(seqfiles[i]);
    }
    fclose(ptr);
}
