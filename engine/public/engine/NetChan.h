#pragma once

#include <deque>
#include <stdint.h>
#include <vector>

#define ENGINE_DEFAULTSVPORT 19771
#define MAX_PACKET_SIZE 1024
#define MAX_RELIABLE 128 // the maximum number of queued up reliables before suspicion
#define STATE_WINDOW 64

namespace engine
{
// this uses a slightly modified version of QW netchan.
// the main changes are removing qport and adding a reliable header to prevent duplicate reads.
class NetChan
{
public:
    // i don't think qport is neccessary anymore.
    // if you have issues, try adding it back.
    typedef struct
    {
        int32_t seq;
        int32_t ack;
    } header_t;
private:
    // returns null if non-existent
    std::vector<uint8_t>* GetBuf(bool unreliable);
public:
    // NetChan is not responsible for creation or destruction of network properties.
    int socket = -1;
    uint8_t ipv4[4] = { 0, 0, 0, 0, };
    uint16_t port = 0;

    int32_t curseq = 0; // used for sequence number
    int32_t lastseen = -1; // last seen sequence from the other end
    int32_t reliableseq = 0; // the index of the reliable being sent
    int32_t lastack = 0; // last of our sequences the other end has acknowledged
    int32_t lastseenreliable = -1; // the last seen reliable index from the other ent

    // a metric of how many packets have been lost between the other and and your end.
    // does NOT count how many packets have been lost from your end to the other end.
    uint32_t nlost = 0;

    std::vector<uint8_t> unreliablebuf;
    std::deque<std::vector<uint8_t>> reliablequeue;

    // last recieved packet
    bool hasreliable = false;
    bool reliablenew = false; // is this the first time we've seen this reliable?
    uint16_t reliablesize;
    int32_t dragmpos = 0;
    std::vector<uint8_t> dgram;

    uint8_t NextUByte(void);
    uint16_t NextUShort(void);
    uint32_t NextUInt(void);
    float NextFloat(void);
    void NextString(char* dest, int len);
    void SkipReliable(void);

    void ClearUnreliable(void);
    bool NewReliable(void);

    // if unreliable is false, write to the newest reliable. else write to unreliable buf
    void WriteUByte(uint8_t ubyte, bool unreliable);
    void WriteUShort(uint16_t ushort, bool unreliable);
    void WriteUInt(uint32_t uint, bool unreliable);
    void WriteFloat(float f, bool unreliable);
    void WriteString(const char* c, int size, bool unreliable);

    void Send(void);
    bool Recieve(const void* data, int datalen);
};
};