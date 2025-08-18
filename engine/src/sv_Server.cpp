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
    int i;

    for(i=0; i<MAX_PLAYER; i++)
        this->clients[i] = NetClient();

    InitNet();
}

void engine::sv::Server::Cleanup(void)
{
    if(this->clsocket >= 0)
        close(this->clsocket);
}

void engine::sv::Server::ProcessUnknownPacket(const uint8_t* data, int datalen, const uint8_t addr[4], int port)
{
    int icl;
    NetClient *cl;

    // check if its a handshake. use an open netchan, but don't confirm the client
    // until you're sure

    icl = FindFreeClient();
    if(icl < 0)
    {
        Console::Print("can't consider connection from %hhu.%hhu.%hhu.%hhu:%d. the server is full!\n",
            addr[0], addr[1], addr[2], addr[3], port);
        return;
    }

    cl = &clients[icl];
    
    *cl = NetClient();
    cl->netchan.socket = this->clsocket;
    memcpy(cl->netchan.ipv4, addr, 4);
    cl->netchan.port = port;

    Console::Print("got packaet2\n");

    // just a random packet
    if(!cl->netchan.Recieve(data, datalen))
        return;

    Console::Print("got packaet1\n");

    if(!cl->netchan.hasreliable)
        return;
    if(cl->netchan.NextUShort() != packet::TYPE_HANDSHAKE)
        return;

    // skip len
    if(cl->netchan.NextUInt() != ENGINE_PACKET_PROTOCOL_VERSION)
        return;
    
    // its a handshake now

    nclients++;
    cl->state = NetClient::NETCLIENT_CONNECTED;
    cl->netchan.NextString(cl->username, ENGINE_PACKET_MAXPLAYERNAME);

    Console::Print("accepted handshake from client %d at %hhu.%hhu.%hhu.%hhu:%d (%s).\n",
        icl, addr[0], addr[1], addr[2], addr[3], port, cl->username);

    // write response

    cl->netchan.NewReliable();
    cl->netchan.WriteUShort(packet::TYPE_HANDSHAKE, false);
    cl->netchan.WriteUByte(icl, false);
}

void engine::sv::Server::ProcessReliablePacket(int icl)
{
    NetClient *cl;

    cl = &clients[icl];
}

bool engine::sv::Server::ProcessUnreliablePacket(int icl)
{
    NetClient *cl;
    uint16_t type, len;
    packet::clsv_playercmd_t playercmd;

    cl = &clients[icl];

    if(!cl->netchan.dgram.size())
        return false;

    if(cl->netchan.dragmpos >= cl->netchan.dgram.size())
        return false;

    type = cl->netchan.NextUShort();
    switch(type)
    {
    case packet::TYPE_PLAYERCMD:
        playercmd.time = cl->netchan.NextUShort();
        playercmd.move = cl->netchan.NextUByte();

        cl->player.ParseCmd(playercmd);
        cl->player.Move((float) playercmd.time / 1000.0);
        break;
    default:
        Console::Print("invalid packet type %d from %hhu.%hhu.%hhu.%hhu:%d (%s).\n",
            type, cl->netchan.ipv4[0], cl->netchan.ipv4[1], cl->netchan.ipv4[2], cl->netchan.ipv4[3], 
            cl->netchan.port, cl->username);
        return false;
    }

    return true;
}

void engine::sv::Server::ProcessRecieved(void)
{
    int i;

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
        if(buflen < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            Console::Print("warning: error with recvfrom on server.\n");
            break;
        }

        if(!buflen)
            continue;

        ipv4 = (uint8_t*) &claddr.sin_addr.s_addr;
        icl = ClientByAddr(ipv4, ntohs(claddr.sin_port));

        if(icl < 0)
            ProcessUnknownPacket(buf, buflen, ipv4, ntohs(claddr.sin_port));
        else
        {
            if(clients[icl].netchan.curseq < clients[icl].netchan.lastseen)
                clients[icl].netchan.curseq = clients[icl].netchan.lastseen;

            clients[icl].netchan.Recieve(buf, buflen);
            if(clients[icl].netchan.hasreliable)
            {
                if(clients[icl].netchan.reliablenew)
                    ProcessReliablePacket(icl);
                else
                    clients[icl].netchan.SkipReliable();
            }
            // only do one since we currently only expect input
            while(ProcessUnreliablePacket(icl));
        }
    } while(1);
}

void engine::sv::Server::SendPackets(void)
{
    int i, j;
    NetClient *cl, *othercl;

    for(i=0, cl=this->clients; i<MAX_PLAYER; i++, cl++)
    {
        if(cl->state == NetClient::NETCLIENT_FREE)
            continue;

        cl->netchan.ClearUnreliable();

        // TODO: check against the client's pvs
        for(j=0, othercl=this->clients; j<MAX_PLAYER; j++, othercl++)
        {
            if(othercl->state == NetClient::NETCLIENT_FREE)
                continue;

            cl->netchan.WriteUShort(packet::TYPE_PLAYERSTATE, true);
            cl->netchan.WriteUByte(j, true);
            cl->netchan.WriteFloat(othercl->player.pos[0], true);
            cl->netchan.WriteFloat(othercl->player.pos[1], true);
        }
        
        cl->netchan.Send();
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

int engine::sv::Server::ClientByAddr(const uint8_t addr[4], uint16_t port)
{
    int i;

    int found;

    for(i=found=0; i<MAX_PLAYER && found<nclients; i++)
    {
        if(this->clients[i].state == NetClient::NETCLIENT_FREE)
            continue;

        found++;

        if(this->clients[i].netchan.port != port)
            continue;
        if(memcmp(this->clients[i].netchan.ipv4, addr, 4))
            continue;

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