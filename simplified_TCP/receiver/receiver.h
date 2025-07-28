#ifndef RECEIVER_H
#define RECEIVER_H

#pragma once 

#include "../common/packet.h"
#include "../channel/unreliable_channel.h"
#include <unordered_map>
#include <cstdint>
#include <unordered_set>

class Receiver{
    public:
        Receiver(UnreliableChannel& ch);

        void process_packets(); // main loop 

    private:
        UnreliableChannel& channel;
        uint32_t expected_seq_num;
        std::unordered_map<uint32_t, Packet> out_of_order_buffer; // avoid dup ACKs

        void process_incoming();
        void deliver_data(const Packet* pkt);
};

#endif