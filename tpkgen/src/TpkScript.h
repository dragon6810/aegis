#pragma once

#include <string>
#include <vector>

#include <scriptlib.h>

class TpkScript
{
private:
    Scriptlib::ScriptFile script;
    std::vector<std::pair<std::string, std::string>> textures; // filename, tpkname

    bool Write(const char* path);
public:
    bool LoadScript(const char* path);
    bool RunScript(void);
};