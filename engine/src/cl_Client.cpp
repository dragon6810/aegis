#include <engine/cl_Client.h>

#include <fcntl.h>
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
            islastframe = true;
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
    int i;
    DumbClient *cl;

    SDL_FRect playersquare;

    for(i=0, cl=this->svclients; i<MAX_PLAYER; i++, cl++)
    {
        if(cl->state != DumbClient::STATE_CONNECTED)
            return;

        playersquare = {};
        SDL_RenderCoordinatesFromWindow(render, cl->player.pos[0], -cl->player.pos[1], &playersquare.x, &playersquare.y);
        playersquare.w = 16;
        playersquare.h = 16;
        SDL_SetRenderDrawColor(render, 128, 128, 128, 255);
        SDL_RenderFillRect(render, &playersquare);
    }
}

void engine::cl::Client::SendPackets(void)
{
    int i;

    gamestate_t *state;

    if(!this->connected && !this->tryconnect)
        return;

    if(this->connected)
    {
        this->netchan.ClearUnreliable();
        this->netchan.WriteUShort(this->pinput->cmd.type, true);
        this->netchan.WriteUShort(this->pinput->cmd.len, true);
        this->netchan.WriteUShort(this->pinput->cmd.time, true);
        this->netchan.WriteUByte(this->pinput->cmd.move, true);
    }

    // copy current
    state = &states[netchan.curseq % STATE_WINDOW];

    for(i=0; i<MAX_PLAYER; i++)
        if(svclients[i].state != DumbClient::STATE_FREE)
            state->svclients[i] = svclients[i];
    state->senttime = TIMEMS;
    state->sequence = netchan.curseq;

    this->netchan.Send();

    states[netchan.lastack % STATE_WINDOW].recievedtime = TIMEMS;
}

void engine::cl::Client::Connect(void)
{
    const char username[ENGINE_PACKET_MAXPLAYERNAME] = "client";

    struct sockaddr_in svaddr;
    socklen_t addrlen;

    Console::Print("connecting to %hhu.%hhu.%hhu.%hhu on port %d.\n", 
        netchan.ipv4[0], netchan.ipv4[1], netchan.ipv4[2], netchan.ipv4[3], netchan.port);

    // set up connection
    if(this->netchan.socket >= 0)
        close(this->netchan.socket);

    this->netchan.socket = socket(PF_INET, SOCK_DGRAM, 0);
    if(this->netchan.socket < 0)
    {
        Console::Print("error %d creating UDP socket on client.\n", errno);
        return;
    }

    if(bind(this->netchan.socket, (const struct sockaddr*) &svaddr, sizeof(svaddr)) < 0)
    {
        Console::Print("error %d binding socket on client.\n", errno);
        exit(1);
    }

    if(getsockname(this->netchan.socket, (struct sockaddr*) &svaddr, &addrlen) < 0)
    {
        Console::Print("error %d with getsockname.\n", errno);
        exit(1);
    }

    this->clport = ntohs(svaddr.sin_port);
    Console::Print("bound socket on port %d.\n", this->clport);

    fcntl(netchan.socket, F_SETFL, fcntl(netchan.socket, F_GETFL, 0) | O_NONBLOCK);

    this->tryconnect = true;

    // write the packet (reliable)
    netchan.NewReliable();
    netchan.WriteUShort(packet::TYPE_HANDSHAKE, false);
    netchan.WriteUInt(ENGINE_PACKET_PROTOCOL_VERSION, false);
    netchan.WriteString(username, ENGINE_PACKET_MAXPLAYERNAME, false);
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

    this->netchan = NetChan();
    memcpy(this->netchan.ipv4, ipv4, 4);
    this->netchan.port = portnum;
    Connect();
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
engine::Console::cvar_t cl_connection_timeout = { "cl_connection_timeout_ms", "2000" };

bool engine::cl::Client::ProcessPacket(void)
{
    uint16_t type, len;
    packet::svcl_playerstate_t playerstate;

    type = netchan.NextUShort();
    len = netchan.NextUShort();

    switch(type)
    {
    case packet::TYPE_HANDSHAKE:
        netchan.NextUByte();
        break;
    case packet::TYPE_SVCL_PLAYERSTATE:
        playerstate = {};
        playerstate.id = netchan.NextUByte();
        playerstate.x = netchan.NextFloat();
        playerstate.y = netchan.NextFloat();

        if(playerstate.id >= MAX_PLAYER)
            return false;

        this->svclients[playerstate.id].player.pos[0] = playerstate.x;
        this->svclients[playerstate.id].player.pos[1] = playerstate.y;
        this->svclients[playerstate.id].state = DumbClient::STATE_CONNECTED;

        break;
    default:
        Console::Print("invalid packet from server.\n");
        return false;
    }

    return true;
}

void engine::cl::Client::ProcessHandshakeResponse(void)
{
    Console::Print("check for handshake\n");
    if(netchan.NextUShort() != packet::TYPE_HANDSHAKE)
        return;

    this->clientid = netchan.NextUByte();

    this->tryconnect = false;
    this->connected = true;
    Console::Print("handshake with server complete.\n");
}

void engine::cl::Client::ProcessRecieved(void)
{
    int i;

    uint8_t buf[MAX_PACKET_SIZE];
    int buflen;
    struct sockaddr_in claddr;
    socklen_t claddrlen;

    if(this->netchan.socket < 0)
        return;

    do
    {
        claddrlen = sizeof(claddr);
        buflen = recvfrom(this->netchan.socket, buf, MAX_PACKET_SIZE, 0, (sockaddr*) &claddr, &claddrlen);
        if(buflen < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            Console::Print("warning: error %d with recvfrom on client.\n", errno);
            break;
        }

        if(!buflen)
            continue;
            
        if(!netchan.Recieve(buf, buflen))
            continue;

        if(!netchan.dgram.size())
            continue;

        // check for handshake response if you're trying to connect to the server.
        if(tryconnect)
        {
            ProcessHandshakeResponse();
            continue;
        }

        while(netchan.dragmpos < netchan.dgram.size())
            if(!ProcessPacket())
                break;
    } while(1);
}

void engine::cl::Client::RecordInput(void)
{
    pinput->GenerateCmd();
    pinput->cmd.time = frametime * 1000;
    states[netchan.curseq % STATE_WINDOW].cmd = pinput->cmd;
}

void engine::cl::Client::PredictLocal(void)
{
    int i;

    int start;
    gamestate_t *state;
    uint64_t netlag, starttime;
    Player *player;

    if(!connected)
        return;

    if(netchan.lastseen < 0)
        return;

    if(netchan.curseq - netchan.lastseen >= STATE_WINDOW)
        return;

    start = netchan.lastseen;

    state = &states[start % STATE_WINDOW];
    netlag = state->recievedtime - state->senttime;

    player = &svclients[clientid].player;
    for(i=start; i<netchan.curseq; i++)
    {
        state = &states[i % STATE_WINDOW];

        player->ParseCmd(states[i % STATE_WINDOW].cmd);
        // Console::Print("wishdir %d: %f %f.\n", i - netchan.lastseen, player.wishdir[0], player.wishdir[1]);
        player->Move((float) states[i % STATE_WINDOW].cmd.time / 1000.0);
    }

    //player->pos = Eigen::Vector2f::Zero();
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
    if(this->netchan.socket >= 0)
    {
        close(this->netchan.socket);
        this->netchan.socket = -1;
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
    
    Init();

    this->MakeWindow();
    render = SDL_CreateRenderer(this->win, NULL);
    SDL_SetRenderVSync(render, 1);
    lastframe = 0;
    while(!this->islastframe)
    {
        thisframe = TIMEMS;
        if(!lastframe)
            lastframe = thisframe;
        frametime = thisframe - lastframe;
        if(frametime < 1)
            frametime = 1;
        frametime /= 1000.0;

        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
        SDL_RenderClear(render);

        this->PollWindow();
        Console::ExecTerm();

        this->ProcessRecieved();

        this->RecordInput();
        
        this->PredictLocal();
        this->DrawClients(render);

        SDL_RenderPresent(render);

        this->SendPackets();

        lastframe = thisframe;
    }
    SDL_DestroyRenderer(render);
    this->DestroyWindow();

    return 0;
}