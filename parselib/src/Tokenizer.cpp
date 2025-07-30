#include <parselib/Tokenizer.h>

#include <mathlib.h>

int Parselib::Tokenizer::TokenLength(char* str, tokentype_e type)
{
    char *c;

    if(!str)
        return 0;
    if(!*str)
        return 0;

    switch(type)
    {
    case TOKEN_IDENTIFIER:
    case TOKEN_NUMBER:
        c = str;
        while(CharValidInToken(c, type, c == str))
            c++;

        return c - str;
    case TOKEN_STRING:
        c = str + 1;
        while(*c != '"' && *c)
            c++;
        if(*c == '"')
            c++;

        return c - str;
    case TOKEN_PUNCTUATION:
        return 1;
    default:
        return 0;
    }
}

bool Parselib::Tokenizer::CharValidInToken(char* c, tokentype_e type, bool firstchar)
{
    switch(type)
    {
    case TOKEN_IDENTIFIER:
        if(INRANGE('a', 'z', *c) || INRANGE('A', 'Z', *c) || *c == '_' || *c == '$')
            return true;

        if(!firstchar && INRANGE('0', '9', *c))
            return true;

        return false;
    case TOKEN_NUMBER:
        if(INRANGE('0', '9', *c))
            return true;

        if(firstchar && *c == '-')
            return true;

        if(!firstchar && *c == '.')
            return true;

        return false;
    case TOKEN_STRING:
        if(*c == '"')
            return true;
        
        if(!firstchar && *c > 32)
            return true;

        return false;
    case TOKEN_PUNCTUATION:
        if(*c > 32)
            return true;

        return false;
    default:
        return false;
    }
}

char* Parselib::Tokenizer::SkipWhitespace(char* c)
{
    if(!c)
        return c;

    while (*c <= 32 && *c)
        c++;

    return c;
}

char* Parselib::Tokenizer::NextToken(char* c)
{
    int i;

    token_t tkn;
    int len;

    if(!c)
        return c;

    if(*c <= 32)
        return NULL;

    tkn = {};
    for(i=0; i<TOKEN_COUNT; i++)
    {
        if(this->CharValidInToken(c, (tokentype_e) i, true))
            break;
    }

    if(i >= TOKEN_COUNT)
        return NULL;

    len = this->TokenLength(c, (tokentype_e) i);

    tkn.type = (tokentype_e) i;
    tkn.val = "";
    tkn.val.append(c, len);
    this->tokens.push_back(tkn);

    return c + len;
}

bool Parselib::Tokenizer::EatFile(const char* path)
{
    FILE *ptr;

    uint64_t filesize;
    std::vector<char> data;
    char *c;

    ptr = fopen(path, "r");
    if(!ptr)
    {
        fprintf(stderr, "Parselib::Tokenizer::EatFile: couldn't open file for reading \"%s\".\n", path);
        return false;
    }

    fseek(ptr, 0, SEEK_END);
    filesize = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);

    data.resize(filesize + 1);
    fread(data.data(), 1, filesize, ptr);
    data[filesize] = 0;
    c = data.data();

    c = this->SkipWhitespace(c);
    while(*c)
    {
        c = this->NextToken(c);
        c = this->SkipWhitespace(c);
        if(!c)
            return false;
    }

    return true;
}