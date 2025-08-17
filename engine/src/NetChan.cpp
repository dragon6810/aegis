#include <engine/NetChan.h>

#include <netinet/in.h>
#include <sys/socket.h>

#include <utilslib.h>

#include <engine/Console.h>

uint8_t engine::NetChan::NextUByte(void)
{
    if(this->dragmpos > this->dgram.size() - sizeof(uint8_t))
        return 0;

    return this->dgram[this->dragmpos++];
}

uint16_t engine::NetChan::NextUShort(void)
{
    uint16_t ushort;

    if(this->dragmpos > this->dgram.size() - sizeof(uint16_t))
        return 0;

    ushort = 0;
    ushort |= (uint16_t) this->dgram[this->dragmpos++] << 8;
    ushort |= (uint16_t) this->dgram[this->dragmpos++];
    
    return ushort;
}

uint32_t engine::NetChan::NextUInt(void)
{
    uint32_t uint;

    if(this->dragmpos > this->dgram.size() - sizeof(uint32_t))
        return 0;

    uint = 0;
    uint |= (uint32_t) this->dgram[this->dragmpos++] << 24;
    uint |= (uint32_t) this->dgram[this->dragmpos++] << 16; 
    uint |= (uint32_t) this->dgram[this->dragmpos++] << 8;
    uint |= (uint32_t) this->dgram[this->dragmpos++];
    
    return uint;
}

float engine::NetChan::NextFloat(void)
{
    union
    {
        uint32_t uint;
        float f;
    } intfloat;

    intfloat.uint = NextUInt();
    return intfloat.f;
}

void engine::NetChan::Send(const void* unreliabledata, int unreliablelen)
{
    header_t *header;
    struct sockaddr_in sockaddr;

    std::vector<uint8_t> message;
    std::vector<uint8_t> unreliable;

    if(socket < 0)
    {
        Console::Print("attempting to send a netchan message with no established connection!\n");
        return;
    }

    message.resize(sizeof(header_t));

    header = (header_t*) message.data();
    *header = {};
    header->magic[0] = 'N';
    header->magic[1] = 'C';
    header->seq = nsent++;
    header->ack = lastseen;

    if(this->reliablequeue.size())
    {
        if(this->reliableseq < 0)
            this->reliableseq = header->seq;
        header->seq |= 0x80000000;
        message.append_range(this->reliablequeue.front());
    }

    if(unreliablelen)
    {
        unreliable.resize(unreliablelen);
        memcpy(unreliable.data(), unreliabledata, unreliablelen);
        message.append_range(unreliable);
    }

    if(message.size() > MAX_PACKET_SIZE)
    {
        Console::Print("warning: packet too large (%d bytes)! aborting send.\n", message.size());
        return;
    }

    sockaddr = {};
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(this->port);
    sockaddr.sin_addr.s_addr = *((uint32_t*)this->ipv4);

    header = (header_t*) message.data();
    header->seq = htonl(header->seq);
    header->ack = htonl(header->ack);

    sendto(this->socket, message.data(), message.size(), 0, (struct sockaddr*) &sockaddr, sizeof(sockaddr));
}

bool engine::NetChan::Recieve(const void* data, int datalen)
{
    header_t header;
    const void *pos;
    int lenleft;

    if(!data || !datalen)
        return false;

    if(datalen < sizeof(header_t))
        return false;

    header = *((header_t*) data);
    pos = (char*) data + sizeof(header_t);
    lenleft = datalen - sizeof(header_t);

    if(header.magic[0] != 'N' || header.magic[1] != 'C')
        return false;

    header.seq = ntohl(header.seq);
    header.ack = ntohl(header.ack);

    this->hasreliable = false;
    if(header.seq & 0x80000000)
    {
        this->hasreliable = true;
        header.seq &= 0x7FFFFFFF;
    }

    if(header.seq <= this->lastseen)
    {
        Console::Print("packet from %hhu.%hhu.%hhu.%hhu:%hu contains invalid netchan header! ignoring.\n",
            this->ipv4[0], this->ipv4[1], this->ipv4[2], this->ipv4[3], this->port);
        Console::Print("seq: %d.\n lastseen: %d.\n", header.seq, this->lastseen);
        return false;
    }

    if(this->lastseen >= 0)
        nlost += header.seq - this->lastseen;
    else
        nlost += header.seq;

    this->lastseen = header.seq;

    // our current reliable command was acknowledged
    if(this->reliableseq >= 0 && header.ack >= this->reliableseq)
    {
        this->reliableseq = -1;
        this->reliablequeue.pop_front();
    }

    this->dragmpos = 0;
    this->dgram.resize(lenleft);
    memcpy(this->dgram.data(), pos, lenleft);

    return true;
}