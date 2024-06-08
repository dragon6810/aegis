#include "rendermodel.h"
#include "loadtexture.h"
#include <glm.hpp>
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include "Quaternion.h"
#include <chrono>

rendermodel::rendermodel(unsigned int shaderProgram)
{
}

void rendermodel::startseq(int seqindex)
{
    curseq = seqindex;
    seqstarttime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void rendermodel::render(const mstudioload& model, float pos[3], GLuint* textures, const mstudioload& texmodel, mstudioseqheader_t** seqheaders)
{
    if (seqstarttime == 0)
        frame = 0.0F;
    else
    {
        long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        float delta = (float)(now - seqstarttime) / 1000.0;
        mstudioseqdescription_t* seq = (mstudioseqdescription_t*)(model.data + model.header.seqindex);
        frame = delta * seq->fps;
    }

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);

    this->model = model;

    mstudiobodypart_t* bodyparts = (mstudiobodypart_t*)(model.data + model.header.bodypartindex);
    mstudiotexture_t* ptextures = (mstudiotexture_t*)(texmodel.data + texmodel.header.textureindex);
    mstudiobone_t* pbones = (mstudiobone_t*)(model.data + model.header.boneindex);
    mstudiobonecontroller_t* pbonecontrollers = (mstudiobonecontroller_t*)(model.data + model.header.bonecontrollerindex);
    
    Mat3x4 boneTransforms[MSTUDIOMAXBONES]{};
    for (int b = 0; b < model.header.numbones; b++)
        boneTransforms[b] = transformfrombone(b, seqheaders);

    for (int b = 0; b < model.header.numbones; b++)
    {
        if (pbones[b].parent == -1)
            continue;
        boneTransforms[b] = boneTransforms[pbones[b].parent] * boneTransforms[b];
    }

    for (int b = 0; b < model.header.numbodyparts; b++)
    {
        mstudiobodypart_t* pbodypart = &bodyparts[b];
        mstudiomodel_t* pmodel = (mstudiomodel_t*)(model.data + pbodypart->modelindex);
        vec3_t* pstudioverts = (vec3_t*)(model.data + pmodel->vertindex);
        ubyte_t* pvertbone = (ubyte_t*)(model.data + pmodel->vertinfoindex);

        vec3_t xformverts[MSTUDIOMAXMESHVERTS]{};
        for (int v = 0; v < pmodel->numverts; v++)
        {
            float coords[3] = { pstudioverts[v].x, pstudioverts[v].y, pstudioverts[v].z };
            Vector3 vec = Vector3(coords);
            vec = boneTransforms[pvertbone[v]] * vec;
            xformverts[v].x = vec.get(0);
            xformverts[v].y = vec.get(1);
            xformverts[v].z = vec.get(2);
        }

        glCullFace(GL_FRONT);

        for (int m = 0; m < pmodel->nummesh; m++)
        {
            mstudiomesh_t* pmesh = (mstudiomesh_t*)(model.data + pmodel->meshindex) + m;
            short* ptricmds = (short*)(model.data + pmesh->triindex);

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
                    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
                    glTexCoord2f((float)ptricmds[2] / (float)ptextures[texindex].width, (float)ptricmds[3] / (float)ptextures[texindex].height);
                    glVertex3f(position.x, position.y, position.z);
                }
                glEnd();
            }
        }
    }

    glDisable(GL_TEXTURE_2D);
}

Mat3x4 rendermodel::transformfrombone(int boneindex, mstudioseqheader_t** seqheaders)
{
    int sequence = curseq;
    int f = (int)frame;
    float s = frame - f;

    mstudiobone_t* pbone = (mstudiobone_t*)(model.data + model.header.boneindex) + boneindex;

    mstudioseqdescription_t* pseqdesc = (mstudioseqdescription_t*)(model.data + model.header.seqindex) + sequence;
    mstudioseqgroup_t* pseqgroup = (mstudioseqgroup_t*)(model.data + model.header.seqgroupindex) + pseqdesc->seqgroup;

    f %= pseqdesc->numframes - 1;

    mstudioanimchunk_t* panim;
    if (pseqdesc->seqgroup == 0)
        panim = (mstudioanimchunk_t*)(model.data + pseqgroup->unused2 + pseqdesc->animindex) + boneindex;
    else
        panim = (mstudioanimchunk_t*)(seqheaders[pseqdesc->seqgroup] + pseqdesc->animindex) + boneindex;

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

rendermodel::~rendermodel()
{
}