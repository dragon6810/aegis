#include "rendermodel.h"

#include "loadtexture.h"

#include <glm.hpp>

#include <iostream>
#include <gtc/matrix_transform.hpp>
#include "Quaternion.h"

rendermodel::rendermodel(unsigned int shaderProgram)
{
	this->shaderProgram = shaderProgram;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void rendermodel::render(const mstudioload& model, float pos[3], GLuint* textures, const mstudioload& texmodel)
{
    glBindVertexArray(VAO);
    mstudiobodypart_t* bodyparts = ((mstudiobodypart_t*) (model.data + model.header.bodypartindex));
    mstudiotexture_t* ptextures = (mstudiotexture_t*)(texmodel.data + texmodel.header.textureindex);

    for (int b = 0; b < model.header.numbodyparts; b++)
    {
        mstudiobodypart_t* pbodypart = bodyparts + b;

        int index = model.header.numbodyparts / pbodypart->base;
        index = index % pbodypart->nummodels;

        mstudiomodel_t* pmodel = (mstudiomodel_t*)(model.data + pbodypart->modelindex) + index;
        vec3_t* normals = (vec3_t*)(model.data + pmodel->normindex);

        mstudiobone_t* pbones = (mstudiobone_t*)(model.data + model.header.boneindex);
        mstudiobonecontroller_t* pbonecontrollers = (mstudiobonecontroller_t*)(model.data + model.header.bonecontrollerindex);

        for (int m = 0; m < pmodel->nummesh; m++)
        {
            mstudiomesh_t* pmesh = (mstudiomesh_t*)(model.data + pmodel->meshindex) + m;
            int numverts = pmesh->numtris < 0 ? -pmesh->numtris : pmesh->numtris;

            Vertex* vertices = (Vertex*) malloc(numverts * sizeof(Vertex));                                 
            int texindex = pmesh->skinref;

            for (int v = 0; v < numverts; v++)
            {
                mstudiotrivert_t* ptrivert = (mstudiotrivert_t*)(model.data + pmesh->triindex) + v;
                ubyte_t* boneindex = (ubyte_t*)(model.data + pmodel->vertinfoindex) + v;
                vec3_t position = *((vec3_t*)(model.data + pmodel->vertindex) + ptrivert->vertindex);
                Vector3 vpos = Vector3(new float[3] {position.x, position.y, position.z});
                Mat3x4 bonetransform = recursetransformfrombone(*boneindex, model.data);
                vpos = bonetransform * vpos;
                vertices[v] = { vpos.get(0), vpos.get(1), vpos.get(2), (float)ptrivert->s / (float)ptextures[texindex].width, (float)ptrivert->t / (float)ptextures[texindex].height};
            }
            
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, numverts * sizeof(Vertex), vertices, GL_DYNAMIC_DRAW);
            glUseProgram(shaderProgram);
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)4 / (float)3, 10.0f, 1000.0f);
            glm::mat4 view = glm::lookAt(glm::vec3(30.0f, 30.0f, 30.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
            unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[texindex]);
            glUniform1i(glGetUniformLocation(shaderProgram, "albedosampler"), 0);
            glDrawArrays(pmesh->numtris < 0 ? GL_TRIANGLE_FAN : GL_TRIANGLE_STRIP, 0, numverts);
            free(vertices);
        }
    }

    glBindVertexArray(0);
}

Mat3x4 rendermodel::transformfrombone(float values[6])
{
    Quaternion axis[3]{};
    axis[STUDIO_XR - 3] = Quaternion::AngleAxis(values[STUDIO_XR], new float[3] {1.0F, 0.0F, 0.0F});
    axis[STUDIO_YR - 3] = Quaternion::AngleAxis(values[STUDIO_YR], new float[3] {0.0F, 1.0F, 0.0F});
    axis[STUDIO_ZR - 3] = Quaternion::AngleAxis(values[STUDIO_ZR], new float[3] {0.0F, 0.0F, 1.0F});

    Quaternion rotation = axis[STUDIO_XR - 3] * axis[STUDIO_YR - 3] * axis[STUDIO_ZR - 3];
    Mat3x4 transform = rotation.toMat();

    transform.val[0][3] = values[STUDIO_X];
    transform.val[1][3] = values[STUDIO_Y];
    transform.val[2][3] = values[STUDIO_Z];

    return transform;
}

Mat3x4 rendermodel::recursetransformfrombone(int bone, char* data)
{
    mstudioheader_t* pheader = (mstudioheader_t*)data;
    mstudiobone_t* pbones = (mstudiobone_t*)(data + pheader->boneindex);

    if (pbones[bone].parent == -1)
        return transformfrombone(pbones[bone].value);

    return recursetransformfrombone(pbones[bone].parent, data) * transformfrombone(pbones[bone].value);
}

rendermodel::~rendermodel()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}