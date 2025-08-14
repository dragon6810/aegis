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

void engine::Console::RegisterCCmd(ccmd_t *ccmd)
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

void engine::Console::ExecStr(const char* str)
{
    const char *c, *argstart;
    std::vector<std::string> args;

    UTILS_ASSERT(str);

    begin:
    args.clear();
    c = argstart = str;
    while(1)
    {
        while(*c <= 32 && *c)
            c++;

        argstart = c;

        if(!*c)
            break;

        // TODO: quoted args should be tolerable to whitespace, then unqote the arg.

        while(*c > 32)
            c++;
        args.push_back(std::string(argstart, c - argstart));
    }

    // silent return because i dont think this should print an error right?
    if(!args.size())
        return;

    if(console.ccmds.find(args[0]) != console.ccmds.end())
    {
        console.ccmds[args[0]]->func(args);
        return;
    }
    if(console.cvars.find(args[0]) != console.cvars.end())
    {
        if(args.size() == 1)
        {
            Print("%s = \"%s\"\n", args[0].c_str(), console.cvars[args[0]]->strval.c_str());
            return;
        }

        if(args.size() != 2)
        {
            Print("expected exactly 0 or 1 argument after CVar.\n");
            return;
        }
        console.cvars[args[0]]->strval = args[1];
        return;
    }

    Print("unknown CCmd/CVar \"%s\".\n", str);
}

void engine::Console::Print(const char* fmt, ...)
{
    va_list		args;
	
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

void engine::Console::LaunchTerm(void)
{
    console.termthread = std::thread([]
    {
        char buf[1024];

        while(1)
        {
            scanf("%1023[^\n]%*c", buf);

            console.termmtx.lock();
            console.termcmds.push_back(buf);
            console.termmtx.unlock();
        }
    });
}

void engine::Console::ExecTerm(void)
{
    console.termmtx.lock();
    while(console.termcmds.size())
    {
        ExecStr(console.termcmds.front().c_str());
        console.termcmds.pop_front();
    }
    console.termmtx.unlock();
}

void engine::Console::KillTerm(void)
{
    console.termthread.join();
}
