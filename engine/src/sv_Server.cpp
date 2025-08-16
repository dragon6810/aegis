#include <engine/sv_Server.h>

#include <fcntl.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>

#include <utilslib.h>

#include <engine/Console.h>
#include <engine/NetChan.h>

engine::sv::Server server;

engine::sv::Server* engine::sv::Server::GetServer()
{
    return &server;
}

void engine::sv::Server::InitNet(void)
{
    struct sockaddr_in svaddr;

    this->clsocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(this->clsocket < 0)
    {
        Console::Print("error creating socket on server.\n");
        exit(1);
    }

    svaddr = {};
    svaddr.sin_family = AF_INET;
    svaddr.sin_addr.s_addr = INADDR_ANY;
    svaddr.sin_port = htons(ENGINE_DEFAULTPORT);
    if(bind(this->clsocket, (const struct sockaddr*) &svaddr, sizeof(svaddr)) < 0)
    {
        Console::Print("error binding socket on server.\n");
        exit(1);
    }
    
    fcntl(this->clsocket, F_SETFL, fcntl(this->clsocket, F_GETFL, 0) | O_NONBLOCK);

    Console::Print("server open on port %hu.\n", ENGINE_DEFAULTPORT);
}

void engine::sv::Server::Init(void)
{
    InitNet();
}

void engine::sv::Server::Cleanup(void)
{
    if(this->clsocket >= 0)
        close(this->clsocket);
}

void engine::sv::Server::ProcessRecieved(void)
{
    uint8_t buf[MAX_PACKET_SIZE];
    int buflen;
    struct sockaddr_in claddr;
    socklen_t claddrlen;

    do
    {
        claddrlen = sizeof(claddr);
        buflen = recvfrom(this->clsocket, buf, MAX_PACKET_SIZE, 0, (sockaddr*) &claddr, &claddrlen);
        if(!buflen)
            continue;
        if(buflen < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            Console::Print("warning: error with recvfrom.\n");
            break;
        }

        Console::Print("potential handshake.\n");
    } while(1);
}

void engine::sv::Server::Setup(void)
{

}

int engine::sv::Server::Run(void)
{
    uint64_t lastframe, thisframe, nextframe, now, time;

    Init();

    // TODO: this is quite imprecise and slow by a few ms per tick.
    // find a better way to do this.

    lastframe = TIMEMS;
    while(!lasttick)
    {
        thisframe = TIMEMS;
        time = thisframe - lastframe;
        lastframe = thisframe;

        Console::ExecTerm();

        ProcessRecieved();

        //printf("tick time: %llu.\n", time);

        nextframe = thisframe + (uint64_t) tickms;
        now = TIMEMS;
        if(now < nextframe)
            usleep((nextframe - now) * 1000);
    }

    Cleanup();

    return 0;
}