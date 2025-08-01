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

        /*
            should be called before loading. if the pair is found in a config file that is loaded,
            the pair will be overwritten by what is in the file.
        */
        void LoadDefault(const char* key, const char* val);
        void Load(const char* path);
        bool Write(const char* path);
    };
};