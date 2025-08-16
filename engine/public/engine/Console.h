#pragma once

#include <deque>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <unordered_map>

namespace engine
{
    class Console
    {
    public:
        typedef struct cvar_s
        {
            const char* name;
            std::string strval;
        } cvar_t;

        typedef struct ccmd_s
        {
            const char* name;
            std::function<void(const std::vector<std::string>&)> func;
        } ccmd_t;
    public:
        std::unordered_map<std::string, cvar_t*> cvars;
        std::unordered_map<std::string, ccmd_t> ccmds;

        std::thread termthread;
        std::mutex termmtx;
        std::deque<std::string> termcmds;
    public:
        static void RegisterCVar(cvar_t *cvar);
        static void RegisterCCmd(const ccmd_t& ccmd);
        // should never be called aside from during ExecTerm. use SubmitStr instead.
        static void ExecStr(const char* str);
        // use this, not ExecStr.
        static void SubmitStr(const char* str);
        static void Print(const char* fmt, ...);

        static void LaunchTerm(void);
        static void ExecTerm(void);
        static void KillTerm(void);
    };
}