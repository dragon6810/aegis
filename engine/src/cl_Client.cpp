#include <engine/cl_Client.h>

#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>

#include <mathlib.h>
#include <utilslib.h>

#include <engine/Console.h>

#include "cl_Input.h"

void engine::cl::Client::MakeWindow(void)
{
    this->win = SDL_CreateWindow("aegis", 640, 360, 0);
}

void engine::cl::Client::PollWindow(void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) 
    {
        switch(event.type)
        {
        case SDL_EVENT_QUIT:
            this->lastframe = true;
            break;
        case SDL_EVENT_KEY_DOWN:
            KeyDown(event.key.scancode);
            break;
        case SDL_EVENT_KEY_UP:
            KeyUp(event.key.scancode);
            break;
        default:
            break;
        }
    }
}

void engine::cl::Client::DestroyWindow(void)
{
    SDL_DestroyWindow(this->win);
    this->win = NULL;
}

void engine::cl::Client::DrawClients(SDL_Renderer* render)
{
    SDL_FRect playersquare;

    playersquare = {};
    SDL_RenderCoordinatesFromWindow(render, this->player.pos[0], -this->player.pos[1], &playersquare.x, &playersquare.y);
    playersquare.w = 16;
    playersquare.h = 16;
    SDL_SetRenderDrawColor(render, 128, 128, 128, 255);
    SDL_RenderFillRect(render, &playersquare);
}

void engine::cl::Client::Connect(const uint8_t svaddr[4], int port)
{
    Console::Print("connecting to %hhu.%hhu.%hhu.%hhu on port %d.\n", svaddr[0], svaddr[1], svaddr[2], svaddr[3], port);

    if(this->svsocket >= 0)
    {
        close(this->svsocket);
        this->svsocket = -1;
    }

    memcpy(this->serveraddr, svaddr, 4);
    this->serverport = port;
    this->connectstart = TIMEMS;
    this->tryconnect = true;
}

void engine::cl::Client::ConnectStr(const std::string& str)
{
    size_t icolon;
    std::string ipstr, portstr;
    uint8_t ipv4[4];
    int portnum;

    icolon = str.find(':');
    if(icolon == std::string::npos)
    {
        ipstr = str;
        portstr = "";
        portnum = ENGINE_DEFAULTSVPORT;
    }
    else
    {
        ipstr = str.substr(0, icolon);
        portstr = str.substr(icolon + 1);
        portnum = std::stoi(portstr);
        if(!INRANGE(1, 65535, portnum))
        {
            Console::Print("invalid port number.\n");
            return;
        }
    }

    if(sscanf(ipstr.c_str(), " %hhu.%hhu.%hhu.%hhu", &ipv4[0], &ipv4[1], &ipv4[2], &ipv4[3]) != 4)
    {
        Console::Print("invald ipv4 address.\n");
        return;
    }

    Connect(ipv4, portnum);
}

void engine::cl::Client::ConnectCmd(const std::vector<std::string>& args)
{
    if(args.size() != 2)
    {
        Console::Print("expected exactly 1 arg.\n");
        return;
    }

    ConnectStr(args[1]);
}

// how long to try to connect to a server before giving up, in ms
engine::Console::cvar_t cl_connection_timeout = { "cl_connection_timeout", "2000" };

void engine::cl::Client::TryConnection(void)
{
    int iconnectiontimeout;
    struct sockaddr_in svaddr;
    packet::clsv_handshake_t handshake;

    iconnectiontimeout = std::stoi(cl_connection_timeout.strval);
    if(TIMEMS - connectstart > iconnectiontimeout)
    {
        Console::Print("connection attempt to server %hhu.%hhu.%hhu.%hhu on port %d timed out.\n",
            serveraddr[0], serveraddr[1], serveraddr[2], serveraddr[3], serverport);
        this->tryconnect = false;
    }

    svaddr = {};
    svaddr.sin_family = AF_INET;
    svaddr.sin_port = htons(ENGINE_DEFAULTCLPORT);
    svaddr.sin_addr.s_addr = *((uint32_t*)this->serveraddr);

    if(this->svsocket < 0)
    {
        this->svsocket = socket(PF_INET, SOCK_DGRAM, 0);
        if(this->svsocket < 0)
        {
            Console::Print("error creating UDP socket on client.\n");
            return;
        }

        if(bind(this->svsocket, (const struct sockaddr*) &svaddr, sizeof(svaddr)) < 0)
        {
            Console::Print("error binding socket on client.\n");
            exit(1);
        }
    }

    svaddr.sin_port = htons(ENGINE_DEFAULTSVPORT);

    handshake = {};
    strcpy(handshake.message, packet::clsv_handshake_message);
    handshake.version = htonl(ENGINE_PACKET_PROTOCOL_VERSION);
    strcpy(handshake.username, "client");

    sendto(this->svsocket, &handshake, sizeof(handshake), 0, (struct sockaddr*) &svaddr, sizeof(svaddr));
}

void engine::cl::Client::ProcessHandshakeResponse(const packet::svcl_handshake_t* packet)
{
    this->tryconnect = false;

    Console::Print("handshake with server complete.\n");
}

void engine::cl::Client::ProcessRecieved(void)
{
    uint8_t buf[MAX_PACKET_SIZE];
    int buflen;
    struct sockaddr_in claddr;
    socklen_t claddrlen;

    if(this->svsocket < 0)
        return;

    do
    {
        claddrlen = sizeof(claddr);
        buflen = recvfrom(this->svsocket, buf, MAX_PACKET_SIZE, 0, (sockaddr*) &claddr, &claddrlen);
        if(!buflen)
            continue;
        if(buflen < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            Console::Print("warning: error with recvfrom on client.\n");
            break;
        }

        if(buflen == sizeof(packet::svcl_handshake_t) &&
           !strncmp((char*) buf, packet::clsv_handshake_message, sizeof(packet::clsv_handshake_message)))
        {
            ProcessHandshakeResponse((packet::svcl_handshake_t*) buf);
            continue;
        }
    } while(1);
}

void engine::cl::Client::Init(void)
{
    InputInit();
    this->pinput->Init();

    Console::RegisterCVar(&cl_connection_timeout);
    Console::RegisterCCmd({ "connect", [this](const std::vector<std::string>& args){ConnectCmd(args);}, });
}

void engine::cl::Client::Cleanup(void)
{
    if(this->svsocket >= 0)
    {
        close(this->svsocket);
        this->svsocket = -1;
    }
}

void engine::cl::Client::Setup(void)
{
    this->pinput = std::make_unique<PlayerInput>(PlayerInput());
}

int engine::cl::Client::Run(void)
{
    // temporary until 3d
    SDL_Renderer *render;
    uint64_t lastframe, thisframe;
    float frametime;

    Init();

    this->MakeWindow();
    render = SDL_CreateRenderer(this->win, NULL);
    lastframe = 0;
    while(!this->lastframe)
    {
        thisframe = TIMEMS;
        if(!lastframe)
            lastframe = thisframe;
        frametime = (float) (thisframe - lastframe) / 1000.0;

        this->PollWindow();
        Console::ExecTerm();

        if(this->tryconnect)
            this->TryConnection();
        this->ProcessRecieved();

        this->player.ParseCmd(this->pinput->GenerateCmd());
        this->player.Move(frametime);

        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
        SDL_RenderClear(render);

        this->DrawClients(render);

        SDL_RenderPresent(render);

        lastframe = thisframe;
    }
    SDL_DestroyRenderer(render);
    this->DestroyWindow();

    return 0;
}