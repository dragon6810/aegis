#include <scriptlib.h>

bool Scriptlib::ScriptFile::ParseFile(const char* path)
{
    Parselib::Tokenizer tknizer;
    Parselib::Tokenizer::token_t *tkn;
    std::optional<ScriptStatement> statement;

    if(!tknizer.EatFile(path))
        return false;

    tkn = tknizer.tokens.data();
    while(tkn->type != Parselib::Tokenizer::TOKEN_EOF)
    {
        statement = ScriptStatement::Parse(&tkn);
        if(!statement.has_value())
            return false;
        this->statements.push_back(statement.value());
    }

    return true;
}