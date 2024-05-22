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

    Mat3x4* boneTransforms = (Mat3x4*) malloc(model.header.numbones * sizeof(Mat3x4));
    for (int b = 0; b < model.header.numbones; b++)
        boneTransforms[b] = recursetransformfrombone(b, model.data);

    for (int b = 0; b < model.header.numbodyparts; b++)
    {
        mstudiobodypart_t* pbodypart = &bodyparts[b];

        int index = model.header.numbodyparts / pbodypart->base;
        index = index % pbodypart->nummodels;

        mstudiomodel_t* pmodel = (mstudiomodel_t*)(model.data + pbodypart->modelindex) + index;
        vec3_t* normals = (vec3_t*)(model.data + pmodel->normindex);

        mstudiobone_t* pbones = (mstudiobone_t*)(model.data + model.header.boneindex);
        mstudiobonecontroller_t* pbonecontrollers = (mstudiobonecontroller_t*)(model.data + model.header.bonecontrollerindex);

        for (int m = 0; m < pmodel->nummesh; m++)
        {
            mstudiomesh_t* pmesh = (mstudiomesh_t*)(model.data + pmodel->meshindex) + m;
            short numverts = *((short*)(model.data + pmesh->triindex));
            short absnumverts = numverts;
            if (numverts < 0)
                absnumverts = -numverts;

            Vertex* vertices = (Vertex*) malloc((absnumverts + 1) * sizeof(Vertex));
            int texindex = pmesh->skinref;

            for (int v = 0; v < absnumverts + 1; v++)
            {
                mstudiotrivert_t* ptrivert = (mstudiotrivert_t*)(model.data + pmesh->triindex + sizeof(short)) + v;
                ubyte_t* boneindex = (ubyte_t*)(model.data + pmodel->vertinfoindex) + ptrivert->vertindex;
                vec3_t position = *((vec3_t*)(model.data + pmodel->vertindex) + ptrivert->vertindex);
                Vector3 vpos = Vector3(new float[3] {position.x, position.y, position.z});
                vpos = boneTransforms[*boneindex] * vpos;
                vertices[v] = { vpos.get(0), vpos.get(1), vpos.get(2), (float)ptrivert->s / (float)ptextures[texindex].width, (float)ptrivert->t / (float)ptextures[texindex].height};
            }
            
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, absnumverts * sizeof(Vertex), vertices, GL_DYNAMIC_DRAW);
            glUseProgram(shaderProgram);
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)4 / (float)3, 10.0f, 1000.0f);
            glm::mat4 view = glm::lookAt(glm::vec3(60.0f, 60.0f, 60.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
            unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[texindex]);
            glUniform1i(glGetUniformLocation(shaderProgram, "albedosampler"), 0);
            glDrawArrays(numverts < 0 ? GL_TRIANGLE_FAN : GL_TRIANGLE_STRIP, 0, numverts);
            free(vertices);
        }
    }

    glBindVertexArray(0);

    free(boneTransforms);
}

Mat3x4 rendermodel::transformfrombone(float values[6], float scales[6])
{
    Quaternion rotation = Quaternion::FromAngle(&values[3]);
    Mat3x4 rotationMat = rotation.toMat();

    Mat3x4 translationMat = Mat3x4::getIdentity();
    translationMat.val[0][3] = values[STUDIO_X];
    translationMat.val[1][3] = values[STUDIO_Y];
    translationMat.val[2][3] = values[STUDIO_Z];

    Mat3x4 transform = translationMat * rotationMat;

    return transform;
}

Mat3x4 rendermodel::recursetransformfrombone(ubyte_t bone, char* data) 
{
    mstudioheader_t* pheader = (mstudioheader_t*)data;
    mstudiobone_t* pbones = (mstudiobone_t*)(data + pheader->boneindex);

    if (pbones[bone].parent == -1)
        return transformfrombone(pbones[bone].value, pbones[bone].scale);

    return recursetransformfrombone(pbones[bone].parent, data) * transformfrombone(pbones[bone].value, pbones[bone].scale);
}

rendermodel::~rendermodel()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}