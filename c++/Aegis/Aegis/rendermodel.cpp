#include "rendermodel.h"
#include "loadtexture.h"
#include <glm.hpp>
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include "Quaternion.h"

rendermodel::rendermodel(unsigned int shaderProgram)
{
}

void rendermodel::render(const mstudioload& model, float pos[3], GLuint* textures, const mstudioload& texmodel)
{


    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);

    mstudiobodypart_t* bodyparts = (mstudiobodypart_t*)(model.data + model.header.bodypartindex);
    mstudiotexture_t* ptextures = (mstudiotexture_t*)(texmodel.data + texmodel.header.textureindex);
    mstudiobone_t* pbones = (mstudiobone_t*)(model.data + model.header.boneindex);
    mstudiobonecontroller_t* pbonecontrollers = (mstudiobonecontroller_t*)(model.data + model.header.bonecontrollerindex);

    Mat3x4 boneTransforms[MSTUDIOMAXBONES];
    for (int b = 0; b < model.header.numbones; b++)
        boneTransforms[b] = transformfrombone(pbones[b].value, pbones[b].scale);

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

        vec3_t* xformverts = (vec3_t*)malloc(sizeof(vec3_t) * pmodel->numverts);
        memcpy(xformverts, pstudioverts, sizeof(vec3_t) * pmodel->numverts);

        for (int v = 0; v < pmodel->numverts; v++)
        {
            Vector3 vec = Vector3(new float[3] { pstudioverts[v].x, pstudioverts[v].y, pstudioverts[v].z });
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
                    glVertex3f(position.x / 10.0F, position.y / 10.0F, position.z / 10.0F);
                }
                glEnd();
            }
        }

        free(xformverts);
    }

    glDisable(GL_TEXTURE_2D);
}

Mat3x4 rendermodel::transformfrombone(float values[6], float scales[6])
{
    Quaternion rotation = Quaternion::FromAngle(&values[3]);
    Mat3x4 rotationMat = rotation.toMat();

    rotationMat.val[0][3] = values[STUDIO_X];
    rotationMat.val[1][3] = values[STUDIO_Y];
    rotationMat.val[2][3] = values[STUDIO_Z];

    return rotationMat;
}

rendermodel::~rendermodel()
{
}