#include <engine/NetChan.h>

#include <netinet/in.h>
#include <sys/socket.h>

#include <utilslib.h>

#include <engine/Console.h>

std::vector<uint8_t>* engine::NetChan::GetBuf(bool unreliable)
{
    if(unreliable)
        return &this->unreliablebuf;

    if(!this->reliablequeue.size())
    {
        Console::Print("attempting to find non-existend reliable buffer!\n");
        return NULL;
    }

    return &this->reliablequeue.back();
}

uint8_t engine::NetChan::NextUByte(void)
{
    if(this->dragmpos + sizeof(uint8_t) > this->dgram.size() + 1)
        return 0;

    return this->dgram[this->dragmpos++];
}

uint16_t engine::NetChan::NextUShort(void)
{
    uint16_t ushort;

    if(this->dragmpos + sizeof(uint16_t) > this->dgram.size())
        return 0;

    ushort = 0;
    ushort |= (uint16_t) this->dgram[this->dragmpos++] << 8;
    ushort |= (uint16_t) this->dgram[this->dragmpos++];
    
    return ushort;
}

uint32_t engine::NetChan::NextUInt(void)
{
    uint32_t uint;

    if(this->dragmpos + sizeof(uint32_t)> this->dgram.size())
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

void engine::NetChan::NextString(char* dest, int len)
{
    if(this->dragmpos + len > this->dgram.size())
    {
        if(dest)
            memset(dest, 0, len);
        return;
    }

    if(strnlen((char*) &dgram[dragmpos], len) >= len)
    {
        // not null-terminated
        memset(dest, 0, len);
        return;
    }

    if(dest)
        memcpy(dest, &dgram[dragmpos], len);
    dragmpos += len;
}

void engine::NetChan::ClearUnreliable(void)
{
    this->unreliablebuf.clear();
}

bool engine::NetChan::NewReliable(void)
{
    if(this->reliablequeue.size() >= MAX_RELIABLE)
        return false;

    this->reliablequeue.push_back({});
    return true;
}

void engine::NetChan::WriteUByte(uint8_t ubyte, bool unreliable)
{
    std::vector<uint8_t> *buf;

    if(!(buf = this->GetBuf(unreliable)))
        return;

    buf->push_back(ubyte);
}

void engine::NetChan::WriteUShort(uint16_t ushort, bool unreliable)
{
    std::vector<uint8_t> *buf;

    if(!(buf = this->GetBuf(unreliable)))
        return;

    buf->reserve(buf->size() + sizeof(uint16_t));
    buf->push_back((ushort & 0xFF00) >> 8);
    buf->push_back((ushort & 0xFF));
}

void engine::NetChan::WriteUInt(uint32_t uint, bool unreliable)
{
    std::vector<uint8_t> *buf;

    if(!(buf = this->GetBuf(unreliable)))
        return;

    buf->reserve(buf->size() + sizeof(uint32_t));
    buf->push_back((uint & 0xFF000000) >> 24);
    buf->push_back((uint & 0xFF0000) >> 16);
    buf->push_back((uint & 0xFF00) >> 8);
    buf->push_back((uint & 0xFF));
}

void engine::NetChan::WriteFloat(float f, bool unreliable)
{
    WriteUInt(*((uint32_t*) &f), unreliable);
}

void engine::NetChan::WriteString(const char* c, int size, bool unreliable)
{
    std::vector<uint8_t> *buf;

    UTILS_ASSERT(c);

    if(!(buf = this->GetBuf(unreliable)))
        return;

    buf->resize(buf->size() + size);
    memcpy(buf->data() + buf->size() - size, c, size);
}

void engine::NetChan::Send(void)
{
    header_t *header;
    struct sockaddr_in sockaddr;

    std::vector<uint8_t> message;

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

    message.append_range(this->unreliablebuf);

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
        return false;
    }

    if(this->lastseen >= 0)
        nlost += header.seq - this->lastseen;
    else
        nlost += header.seq;

    this->lastseen = header.seq;

    //Console::Print("seq: %d, ack: %d.\n", reliableseq, header.ack);

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