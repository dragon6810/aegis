#pragma once

#include <deque>
#include <stdint.h>
#include <vector>

#define ENGINE_DEFAULTCLPORT 19770
#define ENGINE_DEFAULTSVPORT 19771
#define MAX_PACKET_SIZE 1024
#define MAX_RELIABLE 128 // the maximum number of queued up reliables before suspicion

namespace engine
{
// this uses quakeworld's netchan, maybe a more modern implementation
// should be looked into if this doesn't suffice.
class NetChan
{
public:
    // i don't think qport is neccessary anymore.
    // if you have issues, try adding it back.
    typedef struct
    {
        char magic[2]; // 'N' 'C'
        int32_t seq;
        int32_t ack;
    } header_t;
public:
    // NetChan is not responsible for creation or destruction of network properties.
    int socket = -1;
    uint8_t ipv4[4] = { 0, 0, 0, 0, };
    uint16_t port = 0;

    int32_t nsent = 0; // used for sequence number
    int32_t lastseen = -1; // last seen sequence from the other end
    int32_t reliableseq = -1; // the sequence number the current reliable was first sent on

    // a metric of how many packets have been lost between the other and and your end.
    // does NOT count how many packets have been lost from your end to the other end.
    uint32_t nlost = 0;

    std::deque<std::vector<uint8_t>> reliablequeue;

    // last recieved packet
    bool hasreliable = false;
    int32_t dragmpos = 0;
    std::vector<uint8_t> dgram;

    uint8_t NextUByte(void);
    uint16_t NextUShort(void);
    uint32_t NextUInt(void);
    float NextFloat(void);

    void Send(const void* unreliabledata, int unreliablelen);
    bool Recieve(const void* data, int datalen);
};
};