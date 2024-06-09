#include "rendermodel.h"
#include "loadtexture.h"
#include <glm.hpp>
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include "Quaternion.h"
#include "binaryloader.h"
#include <chrono>
#include "AssetManager.h"

void SModel::Load(const char* modelname)
{
    loadBytes(modelname, (char**) &header);

    char texturename[256];
    if (header->numtextures == 0)
    {
        strcpy(texturename, modelname);
        strcpy(&texturename[strlen(texturename) - 4], "T.mdl");
        loadBytes(texturename, (char**)&texheader);
    }
    else
    {
        strcpy(texturename, modelname);
        texheader = header;
    }

    textures = (int*) malloc(sizeof(int) * texheader->numtextures);

    mstudiotexture_t* ptex = (mstudiotexture_t*) ((char*)texheader + texheader->textureindex);
    for (int t = 0; t < texheader->numtextures; t++)
    {
        textures[t] = AssetManager::getInst().getTextureIndex(ptex[t].name, header->name);

        char* texdata = nullptr;
        int width = 0, height = 0;
        loadmstudiotexture((char*)texheader, t, TEXTYPE_MSTUDIO, (int**)&(texdata), &width, &height);
        if (textures[t] < 0)
        {
            textures[t] = -textures[t] + 1;
            glGenTextures(1, (GLuint*)&textures[t]);
            glBindTexture(GL_TEXTURE_2D, textures[t]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata);
        }
        free(texdata);
    }

    if (header->numseqgroups > 1)
    {
        for (int i = 1; i < header->numseqgroups; i++)
        {
            char seqgroupname[256];

            strcpy(seqgroupname, modelname);
            sprintf(&seqgroupname[strlen(seqgroupname) - 4], "%02d.mdl", i);

            loadBytes(&seqgroupname[0], (char**)&seqheader[i]);
        }
    }
}

void SModel::SetPosition(float x, float y, float z)
{
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
}

void SModel::startseq(int seqindex)
{
    curseq = seqindex;
    seqstarttime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void SModel::render()
{
    float camerapos[3]{ 53.0, 53.0, 88.0 };

    if (seqstarttime == 0)
        frame = 0.0F;
    else
    {
        long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        float delta = (float)(now - seqstarttime) / 1000.0;
        mstudioseqdescription_t* seq = (mstudioseqdescription_t*)((char*)header + header->seqindex);
        frame = delta * seq->fps;
    }

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);

    mstudiobodypart_t* bodyparts = (mstudiobodypart_t*)((char*)header + header->bodypartindex);
    mstudiotexture_t* ptextures = (mstudiotexture_t*)((char*)texheader + texheader->textureindex);
    mstudiobone_t* pbones = (mstudiobone_t*)((char*)header + header->boneindex);
    mstudiobonecontroller_t* pbonecontrollers = (mstudiobonecontroller_t*)((char*)header + header->bonecontrollerindex);
       
    for (int b = 0; b < header->numbones; b++)
        boneTransforms[b] = transformfrombone(b);

    for (int b = 0; b < header->numbones; b++)
    {
        if (pbones[b].parent == -1)
            continue;
        boneTransforms[b] = boneTransforms[pbones[b].parent] * boneTransforms[b];
    }

    for (int b = 0; b < header->numbones; b++)
    {
        float up[3] =    { 0.0, 0.0, 1.0 };
        float right[3] = { 1.0, 0.0, 0.0 };

        float bone[3] = { boneTransforms[b].val[0][3], boneTransforms[b].val[1][3], boneTransforms[b].val[2][3] };
        Vector3 bonev = Vector3(bone).normalize();

        Mat3x4 rotonly = boneTransforms[b];
        rotonly.val[0][3] = 0.0;
        rotonly.val[1][3] = 0.0;
        rotonly.val[2][3] = 0.0;

        bonev = rotonly * bonev;

        Vector3 boner = Vector3::cross(Vector3(up), bonev);
        boneright[b].x = boner.get(0);
        boneright[b].y = boner.get(1);
        boneright[b].z = boner.get(2);

        Vector3 boneu = Vector3::cross(bonev, boner);
        boneup[b].x = boneu.get(0);
        boneup[b].y = boneu.get(1);
        boneup[b].z = bonev.get(2);
    }

    for (int b = 0; b < header->numbodyparts; b++)
    {
        mstudiobodypart_t* pbodypart = &bodyparts[b];
        mstudiomodel_t* pmodel = (mstudiomodel_t*)((char*)header + pbodypart->modelindex);
        vec3_t* pstudioverts = (vec3_t*)((char*)header + pmodel->vertindex);
        vec3_t* pstudionorms = (vec3_t*)((char*)header + pmodel->normindex);
        ubyte_t* pvertbone = (ubyte_t*)((char*)header + pmodel->vertinfoindex);
        ubyte_t* pnormbone = (ubyte_t*)((char*)header + pmodel->norminfoindex);
        
        for (int v = 0; v < pmodel->numverts; v++)
        {
            float coords[3] = { pstudioverts[v].x, pstudioverts[v].y, pstudioverts[v].z };
            Vector3 vec = Vector3(coords);
            vec = boneTransforms[pvertbone[v]] * vec;
            xformverts[v].x = vec.get(0);
            xformverts[v].y = vec.get(1);
            xformverts[v].z = vec.get(2);
        }
        
        for (int n = 0; n < pmodel->numnorms; n++)
        {
            float coords[3] = { pstudionorms[n].x, pstudionorms[n].y, pstudionorms[n].z };
            Vector3 vec = Vector3(coords);
            Mat3x4 rotonly = boneTransforms[pnormbone[n]];
            rotonly.val[0][3] = rotonly.val[1][3] = rotonly.val[2][3] = 0.0;
            vec = rotonly * vec;
            xformnorms[n].x = vec.get(0);
            xformnorms[n].y = vec.get(1);
            xformnorms[n].z = vec.get(2);

            float norm[3] = { xformnorms[n].x, xformnorms[n].y, xformnorms[n].z };
            float lightdir[3] = { 0.0, 0.0, 1.0 };
            float ambient = 0.75;
            float light = Vector3::dot(Vector3(lightdir), Vector3(norm)) + ambient;
            lightvals[n].x = light;
            lightvals[n].y = light;
            lightvals[n].z = light;
        }

        glCullFace(GL_FRONT);

        for (int m = 0; m < pmodel->nummesh; m++)
        {
            mstudiomesh_t* pmesh = (mstudiomesh_t*)((char*)header + pmodel->meshindex) + m;

            short* ptricmds = (short*)((char*)header + pmesh->triindex);

            int texindex = pmesh->skinref;
            glBindTexture(GL_TEXTURE_2D, textures[texindex]);
            
            while (int i = *(ptricmds++))
            {
                if (i < 0)
                {
                    i = -i;
                    glBegin(GL_TRIANGLE_FAN);
                }
                else
                {
                    glBegin(GL_TRIANGLE_STRIP);
                }

                for (; i > 0; i--, ptricmds += 4)
                {
                    vec3_t position = xformverts[ptricmds[0]];
                    vec3_t norm = xformnorms[ptricmds[1]];
                    
                    glColor4f(lightvals[ptricmds[1]].x, lightvals[ptricmds[1]].y, lightvals[ptricmds[1]].z, 1.0F);

                    if (ptextures[pmesh->skinref].flags & STUDIO_NF_CHROME)
                    {
                        float posv[3] = { position.x, position.y, position.z };
                        float normv[3] = { norm.x, norm.y, norm.z };
                        Vector3 toeye = Vector3(camerapos) - Vector3(posv);
                        toeye.normalize();
                        Vector3 reflected = Vector3::reflect(toeye, Vector3(normv));
                        float bup[3] = { boneup[pvertbone[ptricmds[0]]].x, boneup[pvertbone[ptricmds[0]]].y, boneup[pvertbone[ptricmds[0]]].z };
                        float bright[3] = { boneright[pvertbone[ptricmds[0]]].x, boneright[pvertbone[ptricmds[0]]].y, boneright[pvertbone[ptricmds[0]]].z };
                        float s = (Vector3::dot(reflected, Vector3(bup)) * ((float) ptextures[texindex].width / 2.0)) + ((float) ptextures[texindex].width / 2.0);
                        float t = (Vector3::dot(reflected, Vector3(bright)) * ((float) ptextures[texindex].height / 2.0)) + ((float) ptextures[texindex].height / 2.0);
                        glTexCoord2f(s / (float)ptextures[texindex].width, t / (float)ptextures[texindex].height);
                    }
                    else
                    {
                        glTexCoord2f((float)ptricmds[2] / (float)ptextures[texindex].width, (float)ptricmds[3] / (float)ptextures[texindex].height);
                    }
                    
                    glVertex3f(position.x + pos[0], position.y + pos[1], position.z + pos[2]);
                }
                glEnd();
            }
        }
    }

    glDisable(GL_TEXTURE_2D);
}

Mat3x4 SModel::transformfrombone(int boneindex)
{
    int sequence = curseq;
    int f = (int)frame;
    float s = frame - f;

    mstudiobone_t* pbone = (mstudiobone_t*)((char*)header + header->boneindex) + boneindex;

    mstudioseqdescription_t* pseqdesc = (mstudioseqdescription_t*)((char*)header + header->seqindex) + sequence;
    mstudioseqgroup_t* pseqgroup = (mstudioseqgroup_t*)((char*)header + header->seqgroupindex) + pseqdesc->seqgroup;

    f %= pseqdesc->numframes - 1;

    mstudioanimchunk_t* panim;
    if (pseqdesc->seqgroup == 0)
        panim = (mstudioanimchunk_t*)((char*)header + pseqgroup->unused2 + pseqdesc->animindex) + boneindex;
    else
        panim = (mstudioanimchunk_t*)(seqheader[pseqdesc->seqgroup] + pseqdesc->animindex) + boneindex;

    float pos[3];
    Quaternion rot;

    float* a1 = (float*) malloc(sizeof(float) * 3);
    float* a2 = (float*) malloc(sizeof(float) * 3);
    for (int i = 0; i < 3; i++)
    {
        if (panim->offset[i + 3] == 0)
        {
            a2[i] = a1[i] = pbone->value[i + 3];
        }
        else
        {
            mstudioanimvalue_t* panimval = (mstudioanimvalue_t*)((char*)panim + panim->offset[i + 3]);
            int j = f;
            while (panimval->num.total <= j)
            {
                j -= panimval->num.total;
                panimval += panimval->num.valid + 1;
            }

            if (panimval->num.valid > j)
            {
                a1[i] = panimval[j + 1].value;

                if (panimval->num.valid > j + 1)
                {
                    a2[i] = panimval[j + 2].value;
                }
                else
                {
                    if (panimval->num.total > j + 1)
                        a2[i] = a1[i];
                    else
                        a2[i] = panimval[panimval->num.valid + 2].value;
                }
            }
            else
            {
                a1[i] = panimval[panimval->num.valid].value;
                if (panimval->num.total > j + 1)
                    a2[i] = a1[i];
                else
                    a2[i] = panimval[panimval->num.valid + 2].value;
            }

            a1[i] = pbone->value[i + 3] + a1[i] * pbone->scale[i + 3];
            a2[i] = pbone->value[i + 3] + a2[i] * pbone->scale[i + 3];
        }

        if (pbone->bonecontroller[i + 3] != -1)
        {
            //a1[i] += adj[pbone->bonecontroller[i + 3]];
        }
    }

    if (a1 == a2)
    {
        rot = Quaternion::FromAngle(a1);
    }
    else
    {
        Quaternion q1 = Quaternion::FromAngle(a1);
        Quaternion q2 = Quaternion::FromAngle(a2);
        rot = Quaternion::Slerp(q1, q2, s);
    }

    for (int i = 0; i < 3; i++)
    {
        pos[i] = pbone->value[i];
        if(panim->offset[i] != 0)
        {
            mstudioanimvalue_t* panimval = (mstudioanimvalue_t*)((char*)panim + panim->offset[i]);

            int j = f;
            while (panimval->num.total <= j)
            {
                j -= panimval->num.total;
                panimval += panimval->num.valid + 1;
            }

            if (panimval->num.valid > j)
            {
                if (panimval->num.valid > j + 1)
                    pos[i] += (panimval[j + 1].value * (1.0F - s) + s * panimval[j + 2].value) * pbone->scale[i];
                else
                    pos[i] += panimval[j + 1].value * pbone->scale[i];
            }
            else
            {
                if (panimval->num.total <= j + 1)
                    pos[i] += (panimval[panimval->num.valid].value * (1.0F - s) + s * panimval[panimval->num.valid + 2].value) * pbone->scale[i];
                else
                    pos[i] += panimval[panimval->num.valid].value * pbone->scale[i];
            }
        }
    }

    if (pseqdesc->motionbone == boneindex)
    {
        if (pseqdesc->motiontype & STUDIO_X)
            pos[0] = 0.0F;
        if (pseqdesc->motiontype & STUDIO_Y)
            pos[1] = 0.0F;
        if (pseqdesc->motiontype & STUDIO_Z)
            pos[2] = 0.0F;
    }

    Mat3x4 rotationMat = rot.toMat();

    rotationMat.val[0][3] = pos[0];
    rotationMat.val[1][3] = pos[1];
    rotationMat.val[2][3] = pos[2];

    free(a1);
    free(a2);

    return rotationMat;
}

SModel::~SModel()
{
    for (int s = 1; s < header->numseqgroups; s++)
        free(seqheader[s]);

    free(textures);
}