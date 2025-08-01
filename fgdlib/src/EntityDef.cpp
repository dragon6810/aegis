#include <fgdlib.h>

Fgdlib::EntityDef::EntityDef()
{
    this->type = ENTTYPE_POINT;
    this->classname = "";
    this->description = "";
    this->pairs.clear();
}

std::optional<Fgdlib::EntityDef> Fgdlib::EntityDef::Load(Parselib::Tokenizer::token_t **tkn)
{
    const char *funcname = "Fgdlib::EntityDef::Load";

    EntityDef entdef;
    std::optional<EntityPair> pair;

    if(!Parselib::Tokenizer::ExpectValue(**tkn, "@", funcname))
        return std::optional<EntityDef>();
    (*tkn)++;

    if(!Parselib::Tokenizer::ExpectType(**tkn, Parselib::Tokenizer::TOKEN_IDENTIFIER, funcname))
        return std::optional<EntityDef>();

    if((*tkn)->val == "PointClass")
        entdef.type = ENTTYPE_POINT;
    else if((*tkn)->val == "SolidClass")
        entdef.type = ENTTYPE_SOLID;
    else
    {
        Parselib::Tokenizer::SyntaxError(**tkn, funcname, "expected valid class type.");
        return std::optional<EntityDef>();
    }

    (*tkn)++;

    // TODO: Class Properties

    if(!Parselib::Tokenizer::ExpectValue(**tkn, "=", funcname))
        return std::optional<EntityDef>();
    (*tkn)++;

    if(!Parselib::Tokenizer::ExpectType(**tkn, Parselib::Tokenizer::TOKEN_IDENTIFIER, funcname))
        return std::optional<EntityDef>();
    entdef.classname = (*tkn)->val;
    (*tkn)++;

    if((*tkn)->val == ":")
    {
        (*tkn)++;
        if(!Parselib::Tokenizer::ExpectType(**tkn, Parselib::Tokenizer::TOKEN_STRING, funcname))
            return std::optional<EntityDef>();
        entdef.description = (*tkn)->val;
        entdef.description.erase(0, 1);
        entdef.description.pop_back();
        (*tkn)++;
    }

    if(!Parselib::Tokenizer::ExpectValue(**tkn, "[", funcname))
        return std::optional<EntityDef>();
    (*tkn)++;

    while((*tkn)->type == Parselib::Tokenizer::TOKEN_IDENTIFIER)
    {
        pair = EntityPair::Load(tkn);
        if(!pair.has_value())
            return std::optional<EntityDef>();
        entdef.pairs.push_back(pair.value());
    }

    if(!Parselib::Tokenizer::ExpectValue(**tkn, "]", funcname))
        return std::optional<EntityDef>();
    (*tkn)++;

    return std::optional<EntityDef>(entdef);
}