#pragma once

#include <optional>
#include <stdio.h>
#include <unordered_map>
#include <vector>

#include <Eigen/Dense>

namespace Tpklib
{
    static constexpr uint32_t max_tex_name = 16; // 15 chars + 1 null terminator
    static constexpr uint32_t palette_size = 256;

    class TpkTex
    {
    public:
        Eigen::Vector2i size;
        std::vector<uint32_t> data; // ARGB
        std::vector<uint8_t> palettedata;
        uint8_t palette[palette_size][3];
    };

    class TpkFile
    {
    private:
        static constexpr char magic[4] = "TPK";
        static constexpr uint32_t version = 1;

        std::string path = "";
        FILE *ptr = NULL;

        #pragma pack(push,1)
        typedef struct
        {
            char magic[4];
            uint32_t version;
            uint32_t texturecount;
            uint64_t tableoffset;
        } header_t;

        typedef struct
        {
            char name[max_tex_name];
            uint16_t size[2]; // width, height
            int compression; // 0 is uncompressed, 1 is Flate.
            uint64_t compressedsize;
            uint64_t uncompressedsize;
            uint64_t dataloc; // Location of data in the file
            uint8_t palette[palette_size][3]; // RGB, if the last color in the palette is 0x0000FF, it is transparent
        } textureindex_t;
        #pragma pack(pop)

        std::optional<TpkTex> GenTexture(const std::vector<uint8_t>& data, const textureindex_t& header);
    public:
        std::string dirname = "";
        std::unordered_map<std::string, TpkTex> tex;

        bool Open(const char* filename);
        void Close(bool cleartextures=false);
        // if name is "", it will load every texture
        bool LoadTex(const char* name);
    };
};