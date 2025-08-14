#pragma once

#include <deque>
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
        typedef enum
        {
            CPERM_ANYONE=0,
            CPERM_CHEATS,
            CPERM_SERVER,
            CPERM_COUNT,
        } cperm_e;

        typedef struct cvar_s
        {
            const char* name;
            cperm_e perm;
            std::string strval;
        } cvar_t;

        typedef struct ccmd_s
        {
            const char* name;
            cperm_e perm;
            void (*func)(const std::vector<std::string>&);
        } ccmd_t;
    public:
        std::unordered_map<std::string, cvar_t*> cvars;
        std::unordered_map<std::string, ccmd_t*> ccmds;

        std::thread termthread;
        std::mutex termmtx;
        std::deque<std::string> termcmds;
    public:
        static void RegisterCVar(cvar_t *cvar);
        static void RegisterCCmd(ccmd_s *ccmd);
        static void ExecStr(const char* str);
        static void Print(const char* fmt, ...);

        static void LaunchTerm(void);
        static void ExecTerm(void);
        static void KillTerm(void);
    };
}