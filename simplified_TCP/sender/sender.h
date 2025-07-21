#ifndef SENDER_H
#define SENDER_H

#include "../common/packet.h"
#include "../channel/unreliable_channel.h"
#include <vector>
#include <unordered_map>

class Sender{
    public:
    Sender(UnreliableChannel& channel, int window_size);
    void send_data(const std::vector<std::string>& messages);
    void receive_acks(); // listen for ACKs and update window

    private:
    UnreliableChannel& channel;
    int window_size;
    uint32_t next_seq_num;
    uint32_t base; // oldest unACK'd packet

    std::unordered_map<uint32_t, Packet> in_flight; // seq_number -> packet
};

#endif