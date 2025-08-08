#pragma once

#include <string>

#define BASECLASSDEF(basename, classname) typedef basename basedef; typedef classname classdef;
#define TIMEMS (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())

namespace Utilslib
{
    std::string StripExtension(const char* str);
    std::string AddExtension(const char* str, const char* ext);
    std::string DefaultExtension(const char* str, const char* ext);
};