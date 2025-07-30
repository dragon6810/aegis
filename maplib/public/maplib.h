#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include <Eigen/Dense>

namespace Maplib
{
    typedef struct
    {
        Eigen::Vector3i triplane[3];
        Eigen::Vector3f texbasis[2];
        float texoffs[2];
        std::string texname;
    } plane_t;

    typedef struct
    {
        std::vector<plane_t> planes;
    } brush_t;

    typedef struct
    {
        std::unordered_map<std::string, std::string> keys;
        std::vector<brush_t> brushes;
    } entity_t;

    class MapFile
    {
    public:
        MapFile();

        std::vector<entity_t> ents;

        bool Write(std::string path);
        static MapFile Load(std::string path);
    };
};