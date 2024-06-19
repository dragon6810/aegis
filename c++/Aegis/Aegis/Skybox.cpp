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
        "bk", // FRONT
        "ft", // BACK
        "up", // UP
        "dn", // DOWN
    };

    std::string fullpath = std::string("valve/gfx/env/") + std::string(sky) + std::string(".bmp");
    GLuint texname = AssetManager::getInst().getTexture(fullpath.c_str(), "sky");
    if (texname != UINT32_MAX)
    {
        this->texname = texname;
        return;
    }
    texname = AssetManager::getInst().setTexture(fullpath.c_str(), "sky");

    for (int i = 0; i < SKYBOX_NUMIMAGES; i++)
    {
        int width;
        int height;
        fullpath = std::string("valve/gfx/env/") + std::string(sky) + prefixes[i] + std::string(".bmp");
        std::vector<int> texdata = LoadBitmap((char*)fullpath.c_str(), i, &width, &height);

        switch (i) // Rotate texture if needed
        {
        case 0:
            texdata = Rotate90(texdata, width, height);
            break;
        case 1:
            texdata = Rotate270(texdata, width, height);
            break;
        case 3:
            texdata = Rotate180(texdata, width, height);
            break;
        case 4:
            texdata = Rotate90(texdata, width, height);
            break;
        default:
            break;
        }
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, texname);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#ifdef TEXTURE_FILTER
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif // TEXTURE_FILTER

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata.data());
    }
    this->texname = texname;
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

std::vector<int> Skybox::Rotate90(const std::vector<int>& texture, int width, int height)
{
    std::vector<int> rotated(height * width);
    for (int y = 0; y < height; ++y) 
    {
        for (int x = 0; x < width; ++x) 
        {
            rotated[x * height + (height - y - 1)] = texture[y * width + x];
        }
    }
    return rotated;
}

std::vector<int> Skybox::Rotate180(const std::vector<int>& texture, int width, int height)
{
    std::vector<int> rotated(width * height);
    for (int y = 0; y < height; ++y) 
    {
        for (int x = 0; x < width; ++x) 
        {
            rotated[(height - y - 1) * width + (width - x - 1)] = texture[y * width + x];
        }
    }
    return rotated;
}

std::vector<int> Skybox::Rotate270(const std::vector<int>& texture, int width, int height)
{
    std::vector<int> rotated(height * width);
    for (int y = 0; y < height; ++y) 
    {
        for (int x = 0; x < width; ++x) 
        {
            rotated[(width - x - 1) * height + y] = texture[y * width + x];
        }
    }
    return rotated;
}

void Skybox::Render() 
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_CUBE_MAP);

    // Front face
    glBindTexture(GL_TEXTURE_CUBE_MAP, texname);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glBegin(GL_QUADS);
    glTexCoord3f(1.0, -1.0, -1.0); glVertex3f(100.0 + campos.x, -100.0 + campos.y, -100.0 + campos.z);
    glTexCoord3f(-1.0, -1.0, -1.0); glVertex3f(-100.0 + campos.x, -100.0 + campos.y, -100.0 + campos.z);
    glTexCoord3f(-1.0, -1.0, 1.0); glVertex3f(-100.0 + campos.x, -100.0 + campos.y, 100.0 + campos.z);
    glTexCoord3f(1.0, -1.0, 1.0); glVertex3f(100.0 + campos.x, -100.0 + campos.y, 100.0 + campos.z);
    glEnd();

    // Back face

    glBegin(GL_QUADS);
    glTexCoord3f(-1.0, 1.0, -1.0); glVertex3f(-100.0 + campos.x, 100.0 + campos.y, -100.0 + campos.z);
    glTexCoord3f(1.0, 1.0, -1.0); glVertex3f(100.0 + campos.x, 100.0 + campos.y, -100.0 + campos.z);
    glTexCoord3f(1.0, 1.0, 1.0); glVertex3f(100.0 + campos.x, 100.0 + campos.y, 100.0 + campos.z);
    glTexCoord3f(-1.0, 1.0, 1.0); glVertex3f(-100.0 + campos.x, 100.0 + campos.y, 100.0 + campos.z);
    glEnd();

    // Top face

    glBegin(GL_QUADS);
    glTexCoord3f(1.0, 1.0, 1.0); glVertex3f(100.0 + campos.x, 100.0 + campos.y, 100.0 + campos.z);
    glTexCoord3f(1.0, -1.0, 1.0); glVertex3f(100.0 + campos.x, -100.0 + campos.y,  100.0 + campos.z);
    glTexCoord3f(-1.0, -1.0, 1.0); glVertex3f(-100.0 + campos.x, -100.0 + campos.y, 100.0 + campos.z);
    glTexCoord3f(-1.0, 1.0, 1.0); glVertex3f(-100.0 + campos.x, 100.0 + campos.y, 100.0 + campos.z);
    glEnd();

    // Bottom face

    glBegin(GL_QUADS);
    glTexCoord3f(1.0, 1.0, -1.0); glVertex3f(100.0f + campos.x, 100.0f + campos.y, -100.0f + campos.z);
    glTexCoord3f(-1.0, 1.0, -1.0); glVertex3f(-100.0f + campos.x, 100.0f + campos.y, -100.0f + campos.z);
    glTexCoord3f(-1.0, -1.0, -1.0); glVertex3f(-100.0f + campos.x, -100.0f + campos.y, -100.0f + campos.z);
    glTexCoord3f(1.0, -1.0, -1.0); glVertex3f(100.0f + campos.x, -100.0f + campos.y, -100.0f + campos.z);
    glEnd();

    // Left face

    glBegin(GL_QUADS);
    glTexCoord3f(1.0, 1.0, -1.0); glVertex3f(100.0f + campos.x, 100.0f + campos.y, -100.0f + campos.z);
    glTexCoord3f(1.0, -1.0, -1.0); glVertex3f(100.0f + campos.x, -100.0f + campos.y, -100.0f + campos.z);
    glTexCoord3f(1.0, -1.0, 1.0); glVertex3f(100.0f + campos.x, -100.0f + campos.y, 100.0f + campos.z);
    glTexCoord3f(1.0, 1.0, 1.0); glVertex3f(100.0f + campos.x, 100.0f + campos.y, 100.0f + campos.z);
    glEnd();

    // Right face
    
    glBegin(GL_QUADS);
    glTexCoord3f(-1.0, -1.0, -1.0); glVertex3f(-100.0f + campos.x, -100.0f + campos.y, -100.0f + campos.z);
    glTexCoord3f(-1.0, 1.0, -1.0); glVertex3f(-100.0f + campos.x, 100.0f + campos.y, -100.0f + campos.z);
    glTexCoord3f(-1.0, 1.0, 1.0); glVertex3f(-100.0f + campos.x, 100.0f + campos.y, 100.0f + campos.z);
    glTexCoord3f(-1.0, -1.0, 1.0); glVertex3f(-100.0f + campos.x, -100.0f + campos.y, 100.0f + campos.z);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_CUBE_MAP);
    glEnable(GL_CULL_FACE);
}