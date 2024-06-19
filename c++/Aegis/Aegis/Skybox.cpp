#include "Skybox.h"

#include <string>
#include <stdio.h>

#include "binaryloader.h"
#include "AssetManager.h"
#include "mathutils.h"

void Skybox::LoadSky(char* sky) {
    const char* prefixes[SKYBOX_NUMIMAGES] = 
    {
        "rt", // RIGHT
        "lf", // LEFT
        "ft", // FRONT
        "bk", // BACK
        "up", // UP
        "dn", // DOWN
    };

    for (int i = 0; i < SKYBOX_NUMIMAGES; i++)
    {
        std::string fullpath = std::string("valve/gfx/env/") + std::string(sky) + std::string(prefixes[i]) + std::string(".bmp");

        int width;
        int height;
        std::vector<int> texdata = LoadBitmap((char*)fullpath.c_str(), i, &width, &height);

        GLuint texname = AssetManager::getInst().getTexture(fullpath.c_str(), "sky");
        if (texname != UINT32_MAX)
        {
            texnames[i] = texname;
            continue;
        }

        texname = AssetManager::getInst().setTexture(fullpath.c_str(), "sky");
        glBindTexture(GL_TEXTURE_2D, texname);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#ifdef TEXTURE_FILTER
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif // TEXTURE_FILTER

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata.data());

        texnames[i] = texname;
    }
}

Skybox::~Skybox() 
{
    for (int i = 0; i < SKYBOX_NUMIMAGES; i++)
        free(mhdr[i]);
}

std::vector<int> Skybox::LoadBitmap(char* path, int whichside, int* width, int* height) 
{
    loadBytes(path, (char**)&mhdr[whichside]);

    bitmapinfoheader_t* infohdr = (bitmapinfoheader_t*)(mhdr[whichside] + 1);

    if (infohdr->bitdepth != 8 || infohdr->compression != 0) 
    {
        printf("*WARNING* Bitmap \"%s\" must be an 8-bit uncompressed bitmap!\n", path);
        *width = 2;
        *height = 2;
        std::vector<int> errtex = { (int)0xFFFF00FF, (int)0xFF000000, (int)0xFFFF00FF, (int)0xFF000000 };
        return errtex;
    }

    int moderncolors[SKYBOX_PALETTESIZE]{};
    for (int i = 0; i < SKYBOX_PALETTESIZE; i++) 
    {
        bitmapcolor_t* col = (bitmapcolor_t*)(infohdr + 1) + i;

        moderncolors[i] = 0xFF000000;
        moderncolors[i] |= col->r << 0;
        moderncolors[i] |= col->g << 8;
        moderncolors[i] |= col->b << 16;
    }

    *width = infohdr->width;
    *height = infohdr->height;
    std::vector<int> texdata(infohdr->width * infohdr->height);
    ubyte_t* pixeldata = (ubyte_t*)(infohdr + 1) + sizeof(bitmapcolor_t) * SKYBOX_PALETTESIZE;
    for (int i = 0; i < infohdr->width * infohdr->height; i++)
        texdata[i] = moderncolors[pixeldata[i]];

    return texdata;
}

void Skybox::Render() 
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    // Front face
    glBindTexture(GL_TEXTURE_2D, texnames[2]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(100.0f + campos.x, -100.0f + campos.y, -100.0f + campos.z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-100.0f + campos.x, -100.0f + campos.y, -100.0f + campos.z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-100.0f + campos.x, -100.0f + campos.y, 100.0f + campos.z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(100.0f + campos.x, -100.0f + campos.y, 100.0f + campos.z);
    glEnd();

    // Back face
    glBindTexture(GL_TEXTURE_2D, texnames[3]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-100.0f + campos.x, 100.0f + campos.y, -100.0f + campos.z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(100.0f + campos.x, 100.0f + campos.y, -100.0f + campos.z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(100.0f + campos.x, 100.0f + campos.y, 100.0f + campos.z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-100.0f + campos.x, 100.0f + campos.y, 100.0f + campos.z);
    glEnd();

    // Top face
    glBindTexture(GL_TEXTURE_2D, texnames[4]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(100.0f + campos.x, 100.0f + campos.y, 100.0f + campos.z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(100.0f + campos.x, -100.0f + campos.y,  100.0f + campos.z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-100.0f + campos.x, -100.0f + campos.y, 100.0f + campos.z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-100.0f + campos.x, 100.0f + campos.y, 100.0f + campos.z);
    glEnd();

    // Bottom face
    glBindTexture(GL_TEXTURE_2D, texnames[5]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(100.0f + campos.x, 100.0f + campos.y, -100.0f + campos.z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-100.0f + campos.x, 100.0f + campos.y, -100.0f + campos.z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-100.0f + campos.x, -100.0f + campos.y, -100.0f + campos.z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(100.0f + campos.x, -100.0f + campos.y, -100.0f + campos.z);
    glEnd();

    // Left face
    glBindTexture(GL_TEXTURE_2D, texnames[0]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(100.0f + campos.x, 100.0f + campos.y, -100.0f + campos.z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(100.0f + campos.x, -100.0f + campos.y, -100.0f + campos.z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(100.0f + campos.x, -100.0f + campos.y, 100.0f + campos.z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(100.0f + campos.x, 100.0f + campos.y, 100.0f + campos.z);
    glEnd();

    // Right face
    glBindTexture(GL_TEXTURE_2D, texnames[1]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-100.0f + campos.x, -100.0f + campos.y, -100.0f + campos.z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-100.0f + campos.x, 100.0f + campos.y, -100.0f + campos.z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-100.0f + campos.x, 100.0f + campos.y, 100.0f + campos.z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-100.0f + campos.x, -100.0f + campos.y, 100.0f + campos.z);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
}