#pragma once

#include <stdio.h>
#include <string>
#include <vector>

namespace Parselib
{
    class Tokenizer
    {
    public:
        typedef enum
        {
            TOKEN_IDENTIFIER=0,
            TOKEN_NUMBER,
            TOKEN_STRING,
            TOKEN_PUNCTUATION,
            TOKEN_COUNT,
        } tokentype_e;

        typedef struct
        {
            std::string val;
            tokentype_e type;
        } token_t;
    private:
        static int TokenLength(char* str, tokentype_e type);
        static bool CharValidInToken(char* c, tokentype_e type, bool firstchar);
        static char* SkipWhitespace(char* c);
        char* NextToken(char* c);
    public:
        std::vector<token_t> tokens;

        /*
            tokenize the given file. returns true if successful.
            it will add onto the existing token array, so clear tokens ahead of time if you want that.
        */
        bool EatFile(const char* path);
    };
}