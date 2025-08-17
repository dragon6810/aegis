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
    svaddr.sin_port = htons(ENGINE_DEFAULTSVPORT);
    if(bind(this->clsocket, (const struct sockaddr*) &svaddr, sizeof(svaddr)) < 0)
    {
        Console::Print("error binding socket on server.\n");
        exit(1);
    }

    fcntl(this->clsocket, F_SETFL, fcntl(this->clsocket, F_GETFL, 0) | O_NONBLOCK);

    Console::Print("server open on port %hu.\n", ENGINE_DEFAULTSVPORT);
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

void engine::sv::Server::ProcessHandshake(const packet::clsv_handshake_t* packet, const uint8_t addr[4])
{
    int icl;
    NetClient *newcl;
    struct sockaddr_in claddr;
    packet::svcl_handshake_t response;
    bool repeat;

    if(packet->version != ntohl(ENGINE_PACKET_PROTOCOL_VERSION))
        return;

    if(strnlen(packet->username, ENGINE_PACKET_MAXPLAYERNAME) >= ENGINE_PACKET_MAXPLAYERNAME)
        return;

    repeat = true;
    icl = this->ClientByAddr(addr);
    if(nclients < MAX_PLAYER || icl >= 0)
    {
        claddr = {};
        claddr.sin_family = AF_INET;
        claddr.sin_port = htons(ENGINE_DEFAULTCLPORT);
        claddr.sin_addr.s_addr = *((uint32_t*)addr);

        if(icl < 0)
        {
            repeat = false;
            icl = FindFreeClient();
        }
        if(icl < 0)
        {
            Console::Print("rejected connection from %hhu.%hhu.%hhu.%hhu because server was full.\n", 
                addr[0], addr[1], addr[2], addr[3]);
            return;
        }

        response = {};
        strcpy(response.message, packet::clsv_handshake_message);
        response.version = htonl(ENGINE_PACKET_PROTOCOL_VERSION);
        response.clid = icl;

        sendto(this->clsocket, &response, sizeof(response), 0, (struct sockaddr*) &claddr, sizeof(claddr));
    }

    // don't make another client for the same one.
    if(repeat)
        return;

    nclients++;
    newcl = &clients[icl];

    newcl->state = NetClient::NETCLIENT_CONNECTED;

    newcl->netchan = NetChan();
    newcl->netchan.socket = this->clsocket;
    memcpy(newcl->netchan.ipv4, addr, 4);
    newcl->netchan.port = ENGINE_DEFAULTCLPORT;

    newcl->player = Player();

    strcpy(newcl->username, packet->username);

    Console::Print("accepted client from %hhu.%hhu.%hhu.%hhu under the username \"%s\".\n",
        addr[0], addr[1], addr[2], addr[3], packet->username);
}

void engine::sv::Server::ProcessClientPacket(int icl, const void* data, int datalen)
{
    NetClient *cl;
    uint16_t type, len;
    packet::clsv_playercmd_t playercmd;

    cl = &clients[icl];

    if(!cl->netchan.Recieve(data, datalen))
        return;

    type = cl->netchan.NextUShort();
    len = cl->netchan.NextUShort();
    switch(type)
    {
    case packet::TYPE_PLAYERCMD:
        playercmd.time = cl->netchan.NextUShort();
        playercmd.move = cl->netchan.NextUByte();

        cl->player.wishdir = Eigen::Vector2f::Zero();
        if(playercmd.move & 0x8)
            cl->player.wishdir[0] -= 1;
        if(playercmd.move & 0x4)
            cl->player.wishdir[0] += 1;
        if(playercmd.move & 0x2)
            cl->player.wishdir[1] -= 1;
        if(playercmd.move & 0x1)
            cl->player.wishdir[1] += 1;
        cl->player.wishdir.normalize();

        cl->player.Move((float) playercmd.time / 1000.0);
        break;
    default:
        goto invalidpacket;
    }

    return;

invalidpacket:

    Console::Print("invalid packet from %hhu.%hhu.%hhu.%hhu (%d).\n",
            cl->netchan.ipv4[0], cl->netchan.ipv4[1], cl->netchan.ipv4[2], cl->netchan.ipv4[3], cl->username);
}

void engine::sv::Server::ProcessRecieved(void)
{
    uint8_t buf[MAX_PACKET_SIZE];
    int buflen;
    struct sockaddr_in claddr;
    socklen_t claddrlen;
    uint8_t *ipv4;
    int icl;

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
            Console::Print("warning: error with recvfrom on server.\n");
            break;
        }

        ipv4 = (uint8_t*) &claddr.sin_addr.s_addr;
        icl = ClientByAddr(ipv4);

        // if it's an unknown client, try a handshake
        if(icl < 0 &&
           buflen == sizeof(packet::clsv_handshake_t) &&
           !strncmp((char*) buf, packet::clsv_handshake_message, sizeof(packet::clsv_handshake_message)))
        {
            ProcessHandshake((packet::clsv_handshake_t*) buf, ipv4);
            continue;
        }

        // random packet
        // TODO: ban ips that send a bunch of these
        if(icl < 0)
            continue;

        ProcessClientPacket(icl, buf, buflen);
    } while(1);
}

void engine::sv::Server::SendPackets(void)
{
    int i;
    NetClient *cl;

    std::vector<uint8_t> unreliable;
    packet::svcl_playerstate_t playerstate;
    union
    {
        uint32_t uint;
        float f;
    } intfloat;

    for(i=0, cl=this->clients; i<MAX_PLAYER; i++, cl++)
    {
        if(cl->state != NetClient::NETCLIENT_CONNECTED)
            continue;
        
        playerstate = {};
        playerstate.type = htons(packet::TYPE_SVCL_PLAYERSTATE);
        playerstate.len = sizeof(playerstate) - 4;
        
        playerstate.id = i;

        intfloat.f = cl->player.pos[0];
        intfloat.uint = htonl(intfloat.uint);
        playerstate.x = intfloat.f;
        intfloat.f = cl->player.pos[1];
        intfloat.uint = htonl(intfloat.uint);
        playerstate.y = intfloat.f;

        intfloat.f = playerstate.x;
        intfloat.uint = ntohl(intfloat.uint);

        Console::Print("player pos: %f %f.\n", intfloat.f, cl->player.pos[1]);

        unreliable.resize(unreliable.size() + sizeof(playerstate));
        memcpy(unreliable.data() + unreliable.size() - sizeof(playerstate), &playerstate, sizeof(playerstate));
    }

    for(i=0, cl=this->clients; i<MAX_PLAYER; i++, cl++)
    {
        if(cl->state != NetClient::NETCLIENT_CONNECTED)
            continue;
        
        cl->netchan.Send(unreliable.data(), unreliable.size());
    }
}

int engine::sv::Server::ClientByAddr(const uint8_t addr[4])
{
    int i;

    int found;

    for(i=found=0; i<MAX_PLAYER && found<nclients; i++)
    {
        if(this->clients[i].state == NetClient::NETCLIENT_FREE)
            continue;

        found++;

        if(!memcmp(this->clients[i].netchan.ipv4, addr, 4))
            return i;
    }

    return -1;
}

int engine::sv::Server::FindFreeClient(void)
{
    int i;

    for(i=0; i<MAX_PLAYER; i++)
        if(this->clients[i].state == NetClient::NETCLIENT_FREE)
            return i;

    return -1;
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

        SendPackets();

        //printf("tick time: %llu.\n", time);

        nextframe = thisframe + (uint64_t) tickms;
        now = TIMEMS;
        if(now < nextframe)
            usleep((nextframe - now) * 1000);
    }

    Cleanup();

    return 0;
}