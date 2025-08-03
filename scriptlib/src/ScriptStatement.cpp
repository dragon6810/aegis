#include <scriptlib.h>

std::optional<Scriptlib::ScriptStatement> Scriptlib::ScriptStatement::Parse(Parselib::Tokenizer::token_t** tkn)
{
    const char *funcname = "Scriptlib::ScriptStatement::Parse";

    ScriptStatement statement;
    std::string arg;

    if((*tkn)->val == "$")
        statement.type = STATEMENT_ASSIGN;
    else if((*tkn)->val == "@")
        statement.type = STATEMENT_CALL;
    else
    {
        Parselib::Tokenizer::SyntaxError(**tkn, funcname, "expected statement.");
        return std::optional<Scriptlib::ScriptStatement>(); 
    }

    (*tkn)++;

    if(!Parselib::Tokenizer::ExpectType(**tkn, Parselib::Tokenizer::TOKEN_IDENTIFIER, funcname))
        return std::optional<Scriptlib::ScriptStatement>(); 
    statement.lident = (*tkn)->val;
    (*tkn)++;

    switch(statement.type)
    {
    case STATEMENT_ASSIGN:
        if(!Parselib::Tokenizer::ExpectValue(**tkn, "=", funcname))
            return std::optional<Scriptlib::ScriptStatement>(); 
        (*tkn++);

        if(!Parselib::Tokenizer::ExpectType(**tkn, Parselib::Tokenizer::TOKEN_STRING, funcname))
            return std::optional<Scriptlib::ScriptStatement>(); 
        statement.args.resize(1);
        statement.args[0] = (*tkn)->val;
        statement.args[0].erase(0, 1);
        statement.args[0].pop_back();
        (*tkn)++;

        break;
    case STATEMENT_CALL:
        if(!Parselib::Tokenizer::ExpectValue(**tkn, "(", funcname))
            return std::optional<Scriptlib::ScriptStatement>(); 
        (*tkn++);

        statement.args.clear();
        while((*tkn)->type == Parselib::Tokenizer::TOKEN_STRING)
        {
            arg = (*tkn)->val;
            arg.erase(0, 1);
            arg.pop_back();
            statement.args.push_back(arg);
            (*tkn)++;

            if((*tkn)->val != ")")
            {
                if(!Parselib::Tokenizer::ExpectValue(**tkn, ",", funcname))
                    return std::optional<Scriptlib::ScriptStatement>(); 
                (*tkn++);
            }
        }

        if(!Parselib::Tokenizer::ExpectValue(**tkn, ")", funcname))
            return std::optional<Scriptlib::ScriptStatement>(); 
        (*tkn++);

        break;
    default:
        break;
    }

    if(!Parselib::Tokenizer::ExpectValue(**tkn, ";", funcname))
            return std::optional<Scriptlib::ScriptStatement>(); 

    return std::optional<Scriptlib::ScriptStatement>(statement); 
}