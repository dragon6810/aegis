#pragma once

#include <optional>
#include <string>

#include <parselib/Tokenizer.h>

namespace Fgdlib
{
    class EntityPair
    {
    public:
        typedef enum
        {
            VALTYPE_STRING=0,
            VALTYPE_INTEGER,
            VALTYPE_FLOAT,
            VALTYPE_FLAGS,
            VALTYPE_CHOICES,
            VALTYPE_COUNT,
        } valtype_e;
    public:
        // can't use a union because of std::string
        // TODO: find a better way to do this, maybe std::variant?
        std::string defstring;
        int defint;
        float deffloat;
        int defflags;
        int defchoices;

        std::string keyname;
        valtype_e type;
        std::string displayname;
        std::string description;

        EntityPair();

        static std::optional<EntityPair> Load(Parselib::Tokenizer::token_t **tkn);
    };

    class EntityDef
    {
    public:
        typedef enum
        {
            ENTTYPE_POINT=0,
            ENTTYPE_SOLID,
            ENTTYPE_COUNT,
        } enttype_e;
    public:
        enttype_e type;
        std::string classname;
        std::string description;
        std::vector<EntityPair> pairs;

        EntityDef();

        static std::optional<EntityDef> Load(Parselib::Tokenizer::token_t **tkn);
    };

    class FgdFile
    {
    public:
        FgdFile();

        std::vector<EntityDef> ents;

        static FgdFile Load(std::string path);
    };
};