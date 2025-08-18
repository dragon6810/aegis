#pragma once

#include <optional>
#include <vector>

#include <parselib/Tokenizer.h>

namespace Scriptlib
{
    class ScriptStatement
    {
    public:
        typedef enum
        {
            STATEMENT_ASSIGN=0,
            STATEMENT_CALL,
            STATEMENT_COUNT,
        } statementtype_e;
    public:
        statementtype_e type;
        std::string lident; // the "left" identifier. name of variable being assigned to or the function being called.
        std::vector<std::string> args; // should be size 1 for variable assignment.

        static std::optional<ScriptStatement> Parse(Parselib::Tokenizer::token_t** tkn);
    };

    class ScriptFile
    {
    public:
        std::vector<ScriptStatement> statements;

        bool ParseFile(const char* path);
    };
}
