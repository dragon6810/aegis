#include <cfglib.h>

#include <parselib/Tokenizer.h>

Cfglib::CfgFile::CfgFile()
{
    this->pairs.clear();
}

void Cfglib::CfgFile::LoadDefault(const char* key, const char* val)
{
    this->pairs[std::string(key)] = std::string(val);
}

void Cfglib::CfgFile::Load(const char* path)
{
    const char *funcname = "Cfglib::CfgFile::Load";

    Parselib::Tokenizer::token_t *tkn;

    Parselib::Tokenizer tknizer;
    std::string key, val;

    tknizer.EatFile(path);
    
    tkn = tknizer.tokens.data();
    while(tkn->type != Parselib::Tokenizer::TOKEN_EOF)
    {
        if(!Parselib::Tokenizer::ExpectType(*tkn, Parselib::Tokenizer::TOKEN_STRING, funcname))
            return;
        key = tkn->val;
        key.erase(0, 1);
        key.pop_back();
        tkn++;

        if(!Parselib::Tokenizer::ExpectValue(*tkn, "=", funcname))
            return;
        tkn++;

        if(!Parselib::Tokenizer::ExpectType(*tkn, Parselib::Tokenizer::TOKEN_STRING, funcname))
            return;
        val = tkn->val;
        val.erase(0, 1);
        val.pop_back();
        tkn++;

        this->pairs[key] = val;
    }
}

bool Cfglib::CfgFile::Write(const char* path)
{
    std::unordered_map<std::string, std::string>::iterator it;

    FILE *ptr;

    ptr = fopen(path, "w");
    if(!ptr)
    {
        printf("Cflib::CfgFile::Write: couldn't open config file for writing \"%s\".\n", path);
        return false;
    }

    for(it=this->pairs.begin(); it!=this->pairs.end(); it++)
        fprintf(ptr, "\"%s\"=\"%s\"\n", it->first.c_str(), it->second.c_str());

    fclose(ptr);
    return true;
}