#pragma once

#include <string>
#include <unordered_map>

namespace Cfglib
{
    class CfgFile
    {
    public:
        std::unordered_map<std::string, std::string> pairs;

        CfgFile();

        void LoadDefault(const char* key, const char* val);
        void Load(const char* path);
        bool Write(const char* path);
    };
};