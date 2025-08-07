#pragma once

#include <string>

#define BASECLASSDEF(basename, classname) typedef basename basedef; typedef classname classdef;

namespace Utilslib
{
    std::string StripExtension(const char* str);
    std::string AddExtension(const char* str, const char* ext);
    std::string DefaultExtension(const char* str, const char* ext);
};