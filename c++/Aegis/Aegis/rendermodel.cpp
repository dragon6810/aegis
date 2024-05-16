#include "rendermodel.h"

#include "loadtexture.h"

#include <glm.hpp>

#include <iostream>
#include <gtc/matrix_transform.hpp>

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

void rendermodel::render(const mstudioload& model, float pos[3], GLuint* textures)
{
    glBindVertexArray(VAO);
    mstudiobodypart_t* bodyparts = ((mstudiobodypart_t*) (model.data + model.header.bodypartindex));
    mstudiotexture_t* ptextures = (mstudiotexture_t*)(model.data + model.header.textureindex);

    for (int b = 0; b < model.header.numbodyparts; b++)
    {
        mstudiobodypart_t* pbodypart = bodyparts + b;

        int index = model.header.numbodyparts / pbodypart->base;
        index = index % pbodypart->nummodels;

        mstudiomodel_t* pmodel = (mstudiomodel_t*)(model.data + pbodypart->modelindex) + index;
        vec3_t* positions = (vec3_t*)(model.data + pmodel->vertindex);

        for (int m = 0; m < pmodel->nummesh; m++)
        {
            mstudiomesh_t* pmesh = (mstudiomesh_t*)(model.data + pmodel->meshindex) + m;
            int numverts = pmesh->numtris < 0 ? -pmesh->numtris : pmesh->numtris;

            Vertex* vertices = (Vertex*) malloc(numverts * sizeof(Vertex));                                 
            int texindex = pmesh->skinref;

            for (int v = 0; v < numverts; v++)
            {
                mstudiotrivert_t* ptrivert = (mstudiotrivert_t*)(model.data + pmesh->triindex) + v;
                vec3_t position = positions[ptrivert->vertindex];
                vertices[v] = { position.x, position.y, position.z, (float)ptrivert->s / (float)ptextures[texindex].width, (float)ptrivert->t / (float)ptextures[texindex].height };
            }

            // Bind the VBO to update the vertex data
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, numverts * sizeof(Vertex), vertices, GL_DYNAMIC_DRAW);

            glUseProgram(shaderProgram);

            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)4 / (float)3, 10.0f, 1000.0f);
            glm::mat4 view = glm::lookAt(glm::vec3(30.0f, 30.0f, 30.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
            unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");

            // Send these matrices to the GPU
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

            GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR)
                std::cerr << "OpenGL error pre swap: " << err << std::endl;

            glActiveTexture(GL_TEXTURE0);
            while ((err = glGetError()) != GL_NO_ERROR)
                std::cerr << "OpenGL error post swap: " << err << std::endl;
            glBindTexture(GL_TEXTURE_2D, textures[texindex]);
            glUniform1i(glGetUniformLocation(shaderProgram, "albedosampler"), 0);

            glDrawArrays(pmesh->numtris < 0 ? GL_TRIANGLE_FAN : GL_TRIANGLE_STRIP, 0, numverts);
            free(vertices);
        }
    }

    glBindVertexArray(0);
}

rendermodel::~rendermodel()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}