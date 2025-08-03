#include <tpklib.h>

#include <zlib.h>

std::optional<Tpklib::TpkTex> Tpklib::TpkFile::GenTexture(const std::vector<uint8_t>& data, const textureindex_t& header)
{
    int i;

    TpkTex tex;
    z_stream inflatestream;

    switch(header.compression)
    {
    case 0:
        // NO COMPRESSION

        tex.palettedata = data;
        break;
    case 1:
        // FLATE

        tex.palettedata.resize(header.uncompressedsize);

        inflatestream = {};
        inflatestream.next_in = const_cast<Bytef*>(data.data());
        inflatestream.avail_in = data.size();
        inflatestream.next_out = tex.palettedata.data();
        inflatestream.avail_out = tex.palettedata.size();

        if (inflateInit2(&inflatestream, -MAX_WBITS) != Z_OK)
        {
            fprintf(stderr, "Tpklib::TpkFile::LoadTex: invalid or corrupted TPK texture \"%s\".\n", header.name);
            return std::optional<Tpklib::TpkTex>();
        }

        if (inflate(&inflatestream, Z_FINISH) != Z_STREAM_END) 
        {
            fprintf(stderr, "Tpklib::TpkFile::LoadTex: invalid or corrupted TPK texture \"%s\".\n", header.name);
            inflateEnd(&inflatestream);
            return std::optional<Tpklib::TpkTex>();
        }

        tex.palettedata.resize(inflatestream.total_out);
        inflateEnd(&inflatestream);

        break;
    default:
        break;
    }

    memcpy(tex.palette, header.palette, sizeof(header.palette));

    tex.data.resize(tex.palettedata.size());
    for(i=0; i<tex.palettedata.size(); i++)
    {
        tex.data[i] = 0;
        tex.data[i] |= ((uint32_t) tex.palette[tex.palettedata[i]][2]) <<  0;
        tex.data[i] |= ((uint32_t) tex.palette[tex.palettedata[i]][1]) <<  8;
        tex.data[i] |= ((uint32_t) tex.palette[tex.palettedata[i]][0]) << 16;
        if(tex.data[i] != 0x000000FF || tex.palettedata[i] != palette_size - 1)
            tex.data[i] |= 0xFF000000;
    }

    return std::optional<Tpklib::TpkTex>(tex);
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
    std::optional<Tpklib::TpkTex> tex;

    if(this->tex.find(std::string(name)) != this->tex.end())
        return true;

    fseek(ptr, 0, SEEK_SET);
    fread(&header, sizeof(header), 1, ptr);

    if(memcmp(this->magic, header.magic, sizeof(header.magic)) || this->version != header.version)
    {
        fprintf(stderr, "Tpklib::TpkFile::LoadTex: invalid, corrupted, or out of date TPK file \"%s\".\n", this->path.c_str());
        return false;
    }

    foundtex = false;
    for(i=0; i<header.texturecount; i++)
    {
        fseek(ptr, header.tableoffset + i * sizeof(textureindex_t), SEEK_SET);
        fread(&texindex, sizeof(texindex), 1, ptr);
        if(name[0] && strncmp(name, texindex.name, max_tex_name))
            continue;

        data.clear();
        data.resize(texindex.compressedsize);
        fseek(ptr, texindex.dataloc, SEEK_SET);
        fread(data.data(), data.size(), 1, ptr);
        tex = this->GenTexture(data, texindex);
        if(!tex.has_value())
            continue;

        this->tex[std::string(texindex.name)] = tex.value();

        foundtex = true;
    }

    return foundtex;
}