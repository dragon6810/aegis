#include <tpklib.h>

Tpklib::TpkTex Tpklib::TpkFile::GenTexture(std::vector<uint8_t> data, const textureindex_t& header)
{

}

bool Tpklib::TpkFile::Open(const char* filename)
{
    if(this->ptr)
        this->Close();

    this->ptr = fopen(filename, "rb");
    if(!this->ptr)
    {
        fprintf(stderr, "Tpklib::TpkFile::Open: couldn't open file for reading \"%s\".\n", filename);
        return false;
    }

    this->path = std::string(filename);
}

void Tpklib::TpkFile::Close(bool cleartextures)
{
    if(!this->ptr)
        return;

    fclose(this->ptr);
    this->path = "";
    this->ptr = NULL;
    if(cleartextures)
        this->tex.clear();
}

bool Tpklib::TpkFile::LoadTex(const char* name)
{
    int i;

    header_t header;
    textureindex_t texindex;
    bool foundtex;
    std::vector<uint8_t> data;

    if(this->tex.find(std::string(name)) != this->tex.end())
        return true;

    fseek(ptr, 0, SEEK_SET);
    fread(&header, sizeof(header), 1, ptr);

    if(memcmp(this->magic, header.magic, sizeof(header.magic)) || this->version != header.version)
    {
        fprintf(stderr, "Tpklib::TpkFile::LoadTex: invalid, corrupted, or out of date TPK file \"%s\".\n", this->path.c_str());
        return;
    }

    foundtex = false;
    for(i=0; i<header.texturecount; i++)
    {
        fseek(ptr, header.tableoffset + i * sizeof(textureindex_t), SEEK_SET);
        fread(&texindex, sizeof(texindex), 1, ptr);
        if(name[0] && strncmp(name, texindex.name, max_tex_name))
            continue;

        data.clear();
        data.resize(texindex.datasize);
        fseek(ptr, texindex.dataloc, SEEK_SET);
        fread(data.data(), data.size(), 1, ptr);
        this->tex[std::string(texindex.name)] = this->GenTexture(data, texindex);

        foundtex = true;
    }

    return foundtex;
}