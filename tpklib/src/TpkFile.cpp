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

    strcpy(tex.name, header.name);
    tex.size[0] = header.size[0];
    tex.size[1] = header.size[1];
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

bool Tpklib::TpkFile::Write(const char* filename, int compress)
{
    int i;
    std::unordered_map<std::string, TpkTex>::iterator it;

    FILE *ptr;
    header_t header;
    std::unordered_map<std::string, textureindex_t> texheaders;
    std::unordered_map<std::string, std::vector<uint8_t>> texdata;
    textureindex_t texheader;
    std::vector<uint8_t> datacompressed;
    uint64_t totalcurrentsize;
    z_stream deflatestream;
    uLong compressedestimate;

    if(compress != 0 && compress != 1)
    {
        fprintf(stderr, "Tpklib::TpkFile::Write: invalid compression option \"%d\".\n", compress);
        return false;
    }

    ptr = fopen(filename, "wb");
    if(!ptr)
    {
        fprintf(stderr, "Tpklib::TpkFile::Write: couldn't open file for writing \"%s\".\n", filename);
        return false;
    }

    totalcurrentsize = sizeof(header_t);
    for(it=this->tex.begin(); it!=this->tex.end(); it++)
    {
        texheader = {};
        strcpy(texheader.name, it->second.name);
        texheader.size[0] = it->second.size[0];
        texheader.size[1] = it->second.size[1];
        texheader.compression = compress;

        datacompressed.clear();
        switch(compress)
        {
        case 0:
            datacompressed = it->second.palettedata;
            break;
        case 1:
            compressedestimate = compressBound(it->second.palettedata.size());
            datacompressed.resize(compressedestimate);

            deflatestream = {};
            deflatestream.next_in = const_cast<Bytef*>(it->second.palettedata.data());
            deflatestream.avail_in = it->second.palettedata.size();
            deflatestream.next_out = datacompressed.data();
            deflatestream.avail_out = datacompressed.size();

            if (deflateInit2(&deflatestream, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK)
            {
                fprintf(stderr, "Tpklib::TpkFile::Write: compression failed for texture \"%s\".\n", texheader.name);
                return false;
            }

            if (deflate(&deflatestream, Z_FINISH) != Z_STREAM_END)
            {
                fprintf(stderr, "Tpklib::TpkFile::Write: compression failed for texture \"%s\".\n", texheader.name);
                return false;
            }

            datacompressed.resize(deflatestream.total_out);
            deflateEnd(&deflatestream);

            printf("compressed texture with compression ratio %f (%lu bytes -> %lu bytes) \"%s\".\n", 
                (float) datacompressed.size() / (float) it->second.palettedata.size(), 
                it->second.palettedata.size(), datacompressed.size(), 
                it->second.name);
            if(datacompressed.size() > it->second.palettedata.size())
                printf("warning: compression ratio > 1 for texture \"%s\". consider disabling compression.\n", texheader.name);

            break;
        default:
            break;
        };

        texheader.uncompressedsize = it->second.palettedata.size();
        texheader.compressedsize = datacompressed.size();
        texheader.dataloc = totalcurrentsize;
        memcpy(texheader.palette, it->second.palette, sizeof(texheader.palette));

        texheaders[std::string(it->second.name)] = texheader;
        texdata[std::string(it->second.name)] = datacompressed;
        
        totalcurrentsize += texheader.compressedsize;
    }

    header = {};
    memcpy(header.magic, magic, sizeof(header.magic));
    header.version = version;
    header.texturecount = this->tex.size();
    header.tableoffset = totalcurrentsize;
    fwrite(&header, sizeof(header), 1, ptr);

    for(it=this->tex.begin(); it!=this->tex.end(); it++)
        fwrite(texdata[std::string(it->second.name)].data(), texdata[std::string(it->second.name)].size(), 1, ptr);

    for(it=this->tex.begin(); it!=this->tex.end(); it++)
        fwrite(&texheaders[std::string(it->second.name)], sizeof(textureindex_t), 1, ptr);

    return true;
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

    return true;
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

        tex.value().filename = std::string(this->path);
        this->tex[std::string(texindex.name)] = tex.value();

        foundtex = true;
    }

    return foundtex;
}