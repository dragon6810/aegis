#include "Wad.h"

#include <c_string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Command.h"
#include "Console.h"

Wad::~Wad()
{
    Unload();
}

void Wad::Open(std::string filename)
{
    FILE* old;
    char magic[4];

    old = ptr;
    filename = Command::datadir + filename;
    ptr = fopen(filename.c_str(), "rb");
    if(!ptr)
    {
        Console::Print("Wad file does not exist \"%s\".\n", filename.c_str());
        ptr = old;
        return;
    }

    fread(magic, 1, 4, ptr);
    if(magic[0] != 'W' || magic[1] != 'A' || magic[2] != 'D' || magic[3] != '3')
    {
        Console::Print("Invalid wad file \"%s\".\n", filename.c_str());
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

    ptr = 0;
}

ResourceManager::texture_t* Wad::LoadTexture(std::string name)
{
    int nmips;

    int i, j;

    ResourceManager::texture_t* tex;
    int ntextures, diroffset;
    char texname[16];
    char errname[17];
    uint32_t w, h;
    std::vector<unsigned int> pixeldata[4];
    unsigned char r[256];
    unsigned char g[256];
    unsigned char b[256];
    unsigned char p;
    char type;
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
        fseek(ptr, 8, SEEK_CUR);
        fread(&type, 1, 1, ptr);
        fseek(ptr, 3, SEEK_CUR);
        fread(texname, 1, sizeof(texname), ptr);
        if(texname[15]) // Not null-terminated
        {
            memcpy(errname, texname, sizeof(texname));
            errname[16] = 0;
            Console::Print("Error in wad: texture name is not null-terminated \"%s\".\n", errname);
            continue;
        }

        if(strcmp(texname, name.c_str()))
            continue;

        break;
    }

    if(i >= ntextures)
    {
        Console::Print("Texture not found in wad \"%s\".\n", name.c_str());
        return NULL;
    }

    if(type != 0x42 && type != 0x43)
    {
        Console::Print("Unknown wad texture type %d.\n", type);
        return NULL;
    }

    nmips = 1;
    if(type == 0x43)
        nmips = 4;

    fseek(ptr, diroffset, SEEK_SET);
    if(type == 0x43)
        fseek(ptr, 16, SEEK_CUR);
        
    fread(&w, sizeof(w), 1, ptr);
    fread(&h, sizeof(h), 1, ptr);
    if(type == 0x43)
    {
        fseek(ptr, 4 * sizeof(int), SEEK_CUR);
        for(i=0, datasize=0; i<nmips; i++)
            datasize += (w>>i) * (h>>i);
    }
    else if(type == 0x42)
        datasize = w * h;
    fseek(ptr, datasize, SEEK_CUR);
    short ncolors;
    fread(&ncolors, 2, 1, ptr);

    for(i=0; i<256; i++)
    {
        fread(b + i, 1, 1, ptr);
        fread(g + i, 1, 1, ptr);
        fread(r + i, 1, 1, ptr);
    }

    fseek(ptr, -256 * 3 - 2 - datasize, SEEK_CUR);
    if(type == 0x43)
    {
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
    }
    else if(type == 0x42)
    {
        pixeldata[0].resize(w * h);
        for(i=0; i<w*h; i++)
        {
            fread(&p, 1, 1, ptr);
            pixeldata[0][i] = 0;
            pixeldata[0][i] |= r[p] <<  0;
            pixeldata[0][i] |= g[p] <<  8;
            pixeldata[0][i] |= b[p] << 16;
            if(pixeldata[0][i] == 0x00FF0000)
                pixeldata[0][i] = 0;
            else
                pixeldata[0][i] |= 0xFF000000;
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

RasterFont Wad::LoadFont(std::string name)
{
    int i;

    RasterFont font;

    int ntextures, diroffset;
    char type;
    char texname[16];
    char errname[17];
    int w, h;
    int nrows;
    unsigned char p;
    unsigned char r[256];
    unsigned char g[256];
    unsigned char b[256];

    std::vector<int> pixeldata;

    if (!ptr)
        return font;

    fseek(ptr, 4, SEEK_SET);
    fread(&ntextures, sizeof(ntextures), 1, ptr);
    fread(&diroffset, sizeof(diroffset), 1, ptr);
    fseek(ptr, diroffset, SEEK_SET);

    for(i=0; i<ntextures; i++)
    {
        fread(&diroffset, sizeof(diroffset), 1, ptr);
        fseek(ptr, 8, SEEK_CUR);
        fread(&type, 1, 1, ptr);
        fseek(ptr, 3, SEEK_CUR);
        fread(texname, 1, sizeof(texname), ptr);

        if(type != 0x46)
            continue;

        if(texname[15]) // Not null-terminated
        {
            memcpy(errname, texname, sizeof(texname));
            errname[16] = 0;
            Console::Print("Error in wad: texture name is not null-terminated \"%s\".\n", errname);
            continue;
        }

        if(strcmp(texname, name.c_str()))
            continue;

        break;
    }

    if(i >= ntextures)
    {
        Console::Print("Couldn't find font \"%s\" in wad.\n", name.c_str());
        return font;
    }

    fseek(ptr, diroffset, SEEK_SET);
    fread(&w, sizeof(int), 1, ptr);
    fread(&h, sizeof(int), 1, ptr);
    fread(&nrows, sizeof(int), 1, ptr);

    if(nrows != 1)
    {
        Console::Print("Error in font \"%s\": Aegis currently only supports 1-row fonts.\n", name.c_str());
        return font;
    }

    fseek(ptr, 4, SEEK_CUR);
    for(i=0; i<256; i++)
    {
        fread(&font.offsets[i], sizeof(short), 1, ptr);
        fread(&font.widths[i], sizeof(short), 1, ptr);
    }

    fseek(ptr, w * h + 2, SEEK_CUR);

    for(i=0; i<256; i++)
    {
        fread(b + i, 1, 1, ptr);
        fread(g + i, 1, 1, ptr);
        fread(r + i, 1, 1, ptr);
    }

    fseek(ptr, -256 * 3 - 2 - w * h, SEEK_CUR);

    pixeldata.resize(w * h);
    for(i=0; i<w*h; i++)
    {
        fread(&p, 1, 1, ptr);
        pixeldata[i] = 0;
        pixeldata[i] |= r[p] <<  0;
        pixeldata[i] |= g[p] <<  8;
        pixeldata[i] |= b[p] << 16;
        if(pixeldata[i] == 0x00000000)
            pixeldata[i] = 0;
        else
            pixeldata[i] |= 0xFF000000;
    }

    font.tex = ResourceManager::FindTexture("wad", name);
    if(!font.tex)
        font.tex = ResourceManager::NewTexture();

    font.tex->source = "wad";
    font.tex->id = name;
    font.tex->width = w;
    font.tex->height = h;

    glBindTexture(GL_TEXTURE_2D, font.tex->name);
        
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
    if(Command::filtertextures)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixeldata.data());

    return font;
}