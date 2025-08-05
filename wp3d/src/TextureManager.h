#pragma once

#include <map>
#include <string>

#include <Eigen/Dense>
#include <GL/glew.h>

#include <tpklib.h>

class TextureManager
{
public:
    typedef struct
    {
        std::string name;
        Eigen::Vector2i size;
        GLuint glid;
        std::vector<uint8_t> data;
        uint8_t palette[Tpklib::palette_size][3];
    } texture_t;

    typedef struct
    {
        std::string path;
        std::map<std::string, texture_t> textures;
    } archive_t;
private:
    texture_t LoadTexture(const Tpklib::TpkTex& tex);
public:
    std::vector<archive_t> archives;

    texture_t* FindTexture(const char* name);

    void LoadArchive(const char* path);
    void ReloadArchive(const char* path);
    void ClearArchive(const char* path);
    void ReloadAll(void);
    void ClearAll(void);
};