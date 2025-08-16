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

void engine::sv::Server::ProcessHandshake(const packet::clsv_handshake_t* packet, const uint8_t addr[4])
{
    int icl;
    NetClient *newcl;
    struct sockaddr_in claddr;
    packet::svcl_handshake_t response;

    if(packet->version != ntohl(ENGINE_PACKET_PROTOCOL_VERSION))
        return;

    if(strnlen(packet->username, ENGINE_PACKET_MAXPLAYERNAME) >= ENGINE_PACKET_MAXPLAYERNAME)
        return;

    icl = this->ClientByAddr(addr);
    if(nclients < MAX_NETCLIENT || icl >= 0)
    {
        Console::Print("send response\n");
        claddr = {};
        claddr.sin_family = AF_INET;
        claddr.sin_port = htons(ENGINE_DEFAULTPORT);
        claddr.sin_addr.s_addr = *((uint32_t*)addr);

        response = {};
        strcpy(response.message, packet::clsv_handshake_message);
        response.version = htonl(ENGINE_PACKET_PROTOCOL_VERSION);

        sendto(this->clsocket, &response, sizeof(response), 0, (struct sockaddr*) &claddr, sizeof(claddr));
    }

    // don't make another client for the same one.
    if(icl >= 0)
        return;

    icl = FindFreeClient();
    if(icl < 0)
    {
        Console::Print("rejected connection from %hhu.%hhu.%hhu.%hhu because server was full.\n", 
            addr[0], addr[1], addr[2], addr[3]);
        return;
    }

    nclients++;
    newcl = &clients[icl];

    newcl->state = NetClient::NETCLIENT_CONNECTED;
    memcpy(newcl->ipv4, addr, 4);
    strcpy(newcl->username, packet->username);

    Console::Print("accepted client from %hhu.%hhu.%hhu.%hhu under the username \"%s\".\n",
        addr[0], addr[1], addr[2], addr[3], packet->username);
}

void engine::sv::Server::ProcessRecieved(void)
{
    uint8_t buf[MAX_PACKET_SIZE];
    int buflen;
    struct sockaddr_in claddr;
    socklen_t claddrlen;
    uint8_t *ipv4;

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

        ipv4 = (uint8_t*) &claddr.sin_addr.s_addr;
        if(buflen == sizeof(packet::clsv_handshake_t) &&
           !strncmp((char*) buf, packet::clsv_handshake_message, sizeof(packet::clsv_handshake_message)))
        {
            ProcessHandshake((packet::clsv_handshake_t*) buf, ipv4);
            continue;
        }
    } while(1);
}

int engine::sv::Server::ClientByAddr(const uint8_t addr[4])
{
    int i;

    int found;

    for(i=found=0; i<MAX_NETCLIENT && found<nclients; i++)
    {
        if(this->clients[i].state == NetClient::NETCLIENT_FREE)
            continue;

        found++;

        if(!memcmp(this->clients[i].ipv4, addr, 4))
            return i;
    }

    return -1;
}

int engine::sv::Server::FindFreeClient(void)
{
    int i;

    for(i=0; i<MAX_NETCLIENT; i++)
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

        //printf("tick time: %llu.\n", time);

        nextframe = thisframe + (uint64_t) tickms;
        now = TIMEMS;
        if(now < nextframe)
            usleep((nextframe - now) * 1000);
    }

    Cleanup();

    return 0;
}