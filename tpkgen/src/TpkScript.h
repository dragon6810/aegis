#pragma once

#include <optional>
#include <string>
#include <vector>

#include <scriptlib.h>
#include <tpklib.h>

class TpkScript
{
private:
    Scriptlib::ScriptFile script;
    std::vector<std::pair<std::string, std::string>> textures; // filename, tpkname

    bool Write(const char* path, int compress);
    static std::optional<Tpklib::TpkTex> LoadBmp(const char* filename, const char* texname);
public:
    bool LoadScript(const char* path);
    bool RunScript(void);
};