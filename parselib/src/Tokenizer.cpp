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

char* Parselib::Tokenizer::NextToken(char* c, const char* str, const char* file)
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
    tkn.posinfile = c - str;
    tkn.file = std::string(file);
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
        this->tokens.push_back({ "", TOKEN_EOF, (int) filesize, std::string(path), });
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
        c = this->NextToken(c, data.data(), path);
        c = this->SkipWhitespace(c);
        if(!c)
            return false;
    }

    this->tokens.push_back({ "", TOKEN_EOF, (int) filesize, std::string(path), });

    return true;
}

void Parselib::Tokenizer::SyntaxError(const token_t& tkn, const char* func, const char* message)
{
    char *c;

    FILE *ptr;
    uint64_t filesize;
    std::vector<char> data;
    int line, col;

    ptr = fopen(tkn.file.c_str(), "r");
    if(!ptr)
    {
        printf("%s: syntax error at (invalid): %s\n", func, message);
        return;
    }

    fseek(ptr, 0, SEEK_END);
    filesize = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);
    data.resize(filesize + 1);
    fread(data.data(), 1, filesize, ptr);
    data[filesize] = 0;
    fclose(ptr);

    line = col = 1;
    c = data.data();
    while(*c && c - data.data() < tkn.posinfile)
    {
        col++;
        if(*c == '\n')
        {
            line++;
            col = 1;
        }
        c++;
    }

    printf("%s: syntax error at %s:%d:%d: %s\n", func, tkn.file.c_str(), line, col, message);
}

bool Parselib::Tokenizer::ExpectValue(const token_t& tkn, std::string val, const char* func)
{
    std::string msg;

    if(tkn.val == val)
        return true;

    msg = std::string("expected \"") + val + std::string("\".");

    SyntaxError(tkn, func, msg.c_str());
    return false;
}

bool Parselib::Tokenizer::ExpectType(const token_t& tkn, tokentype_e type, const char* func)
{
    std::string msg;

    if(tkn.type == type)
        return true;

    msg = std::string("expected \"");
    switch(type)
    {
    case TOKEN_IDENTIFIER:
        msg += "identifier";
        break;
    case TOKEN_NUMBER:
        msg += "number";
        break;
    case TOKEN_STRING:
        msg += "string";
        break;
    case TOKEN_PUNCTUATION:
        msg += "puncuation";
        break;
    case TOKEN_EOF:
        msg += "eof";
        break;
    default:
        msg += "(invalid)";
        break;
    }
    msg += std::string("\".");

    SyntaxError(tkn, func, msg.c_str());
    return false;
}

bool Parselib::Tokenizer::ExpectNotEOF(const token_t& tkn, const char* func)
{
    if(tkn.type != TOKEN_EOF)
        return true;

    SyntaxError(tkn, func, "unexpected eof.");

    return false;
}