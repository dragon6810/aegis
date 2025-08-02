#include <fgdlib.h>

Fgdlib::EntityPair::EntityPair()
{
    this->keyname = "";
}

std::optional<Fgdlib::EntityPair> Fgdlib::EntityPair::Load(Parselib::Tokenizer::token_t **tkn)
{
    const char *funcname = "Fgdlib::EntityDef::Load";

    int i;

    EntityPair pair;

    if(!Parselib::Tokenizer::ExpectType(**tkn, Parselib::Tokenizer::TOKEN_IDENTIFIER, funcname))
        return std::optional<EntityPair>();
    pair.keyname = (*tkn)->val;
    (*tkn)++;

    if(!Parselib::Tokenizer::ExpectValue(**tkn, "(", funcname))
        return std::optional<EntityPair>();
    (*tkn)++;

    if(!Parselib::Tokenizer::ExpectType(**tkn, Parselib::Tokenizer::TOKEN_IDENTIFIER, funcname))
        return std::optional<EntityPair>();
    
    if((*tkn)->val == "string")
        pair.type = VALTYPE_STRING;
    else if((*tkn)->val == "integer")
        pair.type = VALTYPE_INTEGER;
    else if((*tkn)->val == "float")
        pair.type = VALTYPE_FLOAT;
    else if((*tkn)->val == "flags")
    {
        Parselib::Tokenizer::SyntaxError(**tkn, funcname, "TODO: flags.");
        return std::optional<EntityPair>();
        pair.type = VALTYPE_FLAGS;
    }
    else if((*tkn)->val == "choices")
    {
        Parselib::Tokenizer::SyntaxError(**tkn, funcname, "TODO: choices.");
        return std::optional<EntityPair>();
        pair.type = VALTYPE_CHOICES;
    }
    else if((*tkn)->val == "integer3")
        pair.type = VALTYPE_INT3;
    else
    {
        Parselib::Tokenizer::SyntaxError(**tkn, funcname, "expected valid value type.");
        return std::optional<EntityPair>();
    }

    (*tkn)++;

    if(!Parselib::Tokenizer::ExpectValue(**tkn, ")", funcname))
        return std::optional<EntityPair>();
    (*tkn)++;

    if((*tkn)->val == ":" && (*tkn+1)->type == Parselib::Tokenizer::TOKEN_STRING)
    {
        (*tkn)++;
        pair.displayname = (*tkn)->val;
        pair.displayname.erase(0, 1);
        pair.displayname.pop_back();
        (*tkn)++;
    }

    if((*tkn)->val == ":")
        (*tkn)++;

    switch(pair.type)
    {
    case VALTYPE_STRING:
        if((*tkn)->type == Parselib::Tokenizer::TOKEN_STRING)
        {
            pair.defstring = (*tkn)->val;
            pair.defstring.erase(0, 1);
            pair.defstring.pop_back();
            (*tkn)++;
        }
        break;
    case VALTYPE_INTEGER:
        if((*tkn)->type == Parselib::Tokenizer::TOKEN_NUMBER)
        {
            pair.defint = std::stoi((*tkn)->val);
            (*tkn)++;
        }
        break;
    case VALTYPE_FLOAT:
        if((*tkn)->type == Parselib::Tokenizer::TOKEN_NUMBER)
        {
            pair.defint = std::stof((*tkn)->val);
            (*tkn)++;
        }
        break;
    case VALTYPE_INT3:
        if((*tkn)->type != Parselib::Tokenizer::TOKEN_NUMBER)
            break;

        for(i=0; i<3; i++)
        {
            if(!Parselib::Tokenizer::ExpectType(**tkn, Parselib::Tokenizer::TOKEN_NUMBER, funcname))
                return std::optional<Fgdlib::EntityPair>();
            pair.defint3[i] = std::stoi((*tkn)->val);
            (*tkn)++;
        }
        break;
    default:
        Parselib::Tokenizer::SyntaxError(**tkn, funcname, "unsupported default type.");
        return std::optional<EntityPair>();
        break;
    };

    if((*tkn)->val == ":")
    {
        (*tkn)++;
        if(!Parselib::Tokenizer::ExpectType(**tkn, Parselib::Tokenizer::TOKEN_STRING, funcname))
            return std::optional<EntityPair>();

        pair.description = (*tkn)->val;
        pair.description.erase(0, 1);
        pair.description.pop_back();
        (*tkn)++;
    }

    return std::optional<EntityPair>(pair);
}