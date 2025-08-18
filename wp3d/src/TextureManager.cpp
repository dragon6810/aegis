#include "TextureManager.h"

TextureManager::texture_t TextureManager::LoadTexture(const Tpklib::TpkTex& tex)
{
    texture_t newtex;

    newtex = {};
    newtex.name = tex.name;
    newtex.size = tex.size;
    newtex.data = tex.palettedata;
    memcpy(newtex.palette, tex.palette, sizeof(newtex.palette));
    
    glGenTextures(1, &newtex.glid);
    glBindTexture(GL_TEXTURE_2D, newtex.glid);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.size[0], tex.size[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.data.data());

    return newtex;
}

TextureManager::texture_t* TextureManager::FindTexture(const char* name)
{
    int i;
    std::map<std::string, texture_t>::iterator it;

    for(i=0; i<this->archives.size(); i++)
    {
        if(this->archives[i].textures.find(name) == this->archives[i].textures.end())
            continue;

        return &this->archives[i].textures[name];
    }

    return NULL;
}

GLuint TextureManager::GetMissingTextureID(void)
{
    // TODO: return an actual error texture
    return 0;
}

void TextureManager::LoadArchive(const char* path)
{
    Tpklib::TpkFile file;
    std::unordered_map<std::string, Tpklib::TpkTex>::iterator it;

    archive_t archive;

    if(!file.Open(path))
        return;

    if(!file.LoadTex(""))
        return;

    file.Close();

    archive = {};
    archive.path = path;
    
    for(it=file.tex.begin(); it!=file.tex.end(); it++)
        archive.textures[it->second.name] = this->LoadTexture(it->second);

    this->archives.push_back(archive);
}

void TextureManager::ReloadArchive(const char* path)
{
    int i;

    int iarchive;

    archive_t archive;

    for(i=0; i<this->archives.size(); i++)
        if(!strcmp(this->archives[i].path.c_str(), path))
            break;

    if(i >= this->archives.size())
    {
        fprintf(stderr, "TextureManager::Reload: couldn't locate archive \"%s\".\n", path);
        return;
    }

    iarchive = i;

    ClearArchive(path);
    LoadArchive(path);

    // LoadArchive will put it at the end, so we have to restore it's initial position.
    archive = this->archives.back();
    this->archives.pop_back();
    this->archives.insert(this->archives.begin() + iarchive, archive);
}

void TextureManager::ClearArchive(const char* path)
{
    int i;
    std::map<std::string, texture_t>::iterator it;

    int iarchive;
    archive_t *archive;

    for(i=0; i<this->archives.size(); i++)
        if(!strcmp(this->archives[i].path.c_str(), path))
            break;

    if(i >= this->archives.size())
    {
        fprintf(stderr, "TextureManager::ClearArchive: couldn't locate archive \"%s\".\n", path);
        return;
    }

    iarchive = i;
    archive = &this->archives[iarchive];

    for(it=archive->textures.begin(); it!=archive->textures.end(); it++)
        glDeleteTextures(1, &it->second.glid);
    
    this->archives.erase(this->archives.begin() + iarchive);
}

void TextureManager::ReloadAll(void)
{
    int i;

    int narchives;

    for(i=0; i<this->archives.size(); i++)
        this->ReloadArchive(this->archives[i].path.c_str());
}

void TextureManager::ClearAll(void)
{
    while(this->archives.size())
        this->ClearArchive(this->archives.back().path.c_str());
}