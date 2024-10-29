#include "Wad.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Command.h"

void Wad::Open(std::string filename)
{
    FILE* old;
    char magic[4];

    old = ptr;
    filename = Command::datadir + filename;
    ptr = fopen(filename.c_str(), "rb");
    if(!ptr)
    {
        printf("Wad file does not exist \"%s\".\n", filename.c_str());
        ptr = old;
        return;
    }

    fread(magic, 1, 4, ptr);
    if(magic[0] != 'W' || magic[1] != 'A' || magic[2] != 'D' || magic[3] != '3')
    {
        printf("Invalid wad file \"%s\".\n", filename.c_str());
        fclose(ptr);
        ptr = old;
        return;
    }

    if(old)
        fclose(old);
}

void Wad::Unload()
{
    if(ptr)
        fclose(ptr);
}

ResourceManager::texture_t* Wad::LoadTexture(std::string name)
{
    const int nmips = 4;

    int i, j;

    ResourceManager::texture_t* tex;
    int ntextures, diroffset;
    char texname[16];
    char errname[17];
    uint32_t w, h;
    std::vector<int> pixeldata[nmips];
    char r[256];
    char g[256];
    char b[256];
    unsigned char p;
    int datasize;

    if (!ptr)
        return NULL;

    fseek(ptr, 4, SEEK_SET);
    fread(&ntextures, sizeof(ntextures), 1, ptr);
    fread(&diroffset, sizeof(diroffset), 1, ptr);
    fseek(ptr, diroffset, SEEK_SET);

    for(i=0; i<ntextures; i++)
    {
        fread(&diroffset, sizeof(diroffset), 1, ptr);
        fseek(ptr, 12, SEEK_CUR);
        fread(texname, 1, sizeof(texname), ptr);
        if(texname[15]) // Not null-terminated
        {
            memcpy(errname, texname, sizeof(texname));
            errname[16] = 0;
            printf("Error in wad: texture name is not null-terminated \"%s\".\n", errname);
            continue;
        }

        if(strcmp(texname, name.c_str()))
            continue;

        break;
    }

    if(i >= ntextures)
    {
        printf("Texture not found in wad \"%s\".\n", name.c_str());
        return NULL;
    }

    fseek(ptr, diroffset + 16, SEEK_SET);
    fread(&w, sizeof(w), 1, ptr);
    fread(&h, sizeof(h), 1, ptr);
    fseek(ptr, 4 * sizeof(int), SEEK_CUR);
    for(i=0, datasize=0; i<nmips; i++)
        datasize += (w>>i) * (h>>i);
    fseek(ptr, datasize + 2, SEEK_CUR);

    for(i=0; i<256; i++)
    {
        fread(b + i, 1, 1, ptr);
        fread(g + i, 1, 1, ptr);
        fread(r + i, 1, 1, ptr);
    }

    fseek(ptr, -256 * 3 - 2 - datasize, SEEK_CUR);
    for(i=0; i<nmips; i++)
    {
        pixeldata[i].resize((w>>i)*(h>>i));
        for(j=0; j<(w>>i)*(h>>i); j++)
        {
            fread(&p, 1, 1, ptr);
            pixeldata[i][j] = 0;
            pixeldata[i][j] |= r[p] <<  0;
            pixeldata[i][j] |= g[p] <<  8;
            pixeldata[i][j] |= b[p] << 16;
            if(pixeldata[i][j] == 0x00FF0000)
                pixeldata[i][j] = 0;
            else
                pixeldata[i][j] |= 0xFF000000;
        }
    }

    tex = ResourceManager::FindTexture("wad", name);
    if(!tex)
        tex = ResourceManager::NewTexture();

    tex->source = "wad";
    tex->id = name;
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, nmips - 1);
    
    for(i=0; i<nmips; i++)
        glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, w >> i, h >> i, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixeldata[i].data());

    return tex;
}