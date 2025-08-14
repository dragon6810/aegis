#include <engine/Console.h>

#include <utilslib.h>

engine::Console console = engine::Console();

void engine::Console::RegisterCVar(cvar_t *cvar)
{
    UTILS_ASSERT(cvar);
    UTILS_ASSERT(cvar->name);

    if(console.ccmds.find(cvar->name) != console.ccmds.end())
    {
        Print("can't add cvar for it already exists as CCmd \"%s\".\n", cvar->name);
        return;
    }

    if(console.cvars.find(cvar->name) != console.cvars.end())
    {
        Print("can't add cvar for it already exists as CVar \"%s\".\n", cvar->name);
        return;
    }

    console.cvars[cvar->name] = cvar;
}

void engine::Console::RegisterCCmd(ccmd_s *ccmd)
{
    UTILS_ASSERT(ccmd);
    UTILS_ASSERT(ccmd->name);
    UTILS_ASSERT(ccmd->func);

    if(console.ccmds.find(ccmd->name) != console.ccmds.end())
    {
        Print("can't add cvar for it already exists as CCmd \"%s\".\n", ccmd->name);
        return;
    }

    if(console.cvars.find(ccmd->name) != console.cvars.end())
    {
        Print("can't add cvar for it already exists as CVar \"%s\".\n", ccmd->name);
        return;
    }

    console.ccmds[ccmd->name] = ccmd;
}

void engine::Console::Print(const char* fmt, ...)
{
    va_list		args;
	
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}