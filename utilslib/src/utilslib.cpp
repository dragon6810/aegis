#include <utilslib.h>

#include <assert.h>

std::string Utilslib::StripExtension(const char* str)
{
    std::string stripped;

    assert(str && "Stdlib::StripExtension: str is NULL!");

    const char *c;

    c = str + strlen(str) - 1;
    while(c >= str && *c != '.')
        c--;
    if(c <= str)
        return str;

    stripped = std::string(str, c - str);
    return stripped;
}

std::string Utilslib::AddExtension(const char* str, const char* ext)
{
    std::string withext;

    assert(str && "Stdlib::AddExtension: str is NULL!");

    withext = std::string(str) + "." + std::string(ext);
    return withext;
}

std::string Utilslib::DefaultExtension(const char* str, const char* ext)
{
    std::string stripped;

    assert(str && "Stdlib::DefaultExtension: str is NULL!");

    stripped = StripExtension(str);
    if(strcmp(stripped.c_str(), str))
        return str;

    return stripped + "." + ext;
}