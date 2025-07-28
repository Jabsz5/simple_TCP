#ifndef SENDER_H
#define SENDER_H

#include "../common/packet.h"
#include "../channel/unreliable_channel.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

class Sender{
    public:
    Sender(UnreliableChannel& channel, int window_size, int timeout);
    void send_data(const std::vector<std::string>& messages);
    void receive_acks(); // listen for ACKs and update window
    void check_timeouts();
    bool is_done(size_t total_packets);
    const std::unordered_set<uint32_t>& get_failed_packets() const {
        return failed_packets;
    }

    private:
    int timeout;
    UnreliableChannel& channel;
    int window_size;
    uint32_t next_seq_num;
    uint32_t base; // oldest unACK'd packet

    std::unordered_map<uint32_t, Packet> in_flight; // seq_number -> packet
    std::unordered_map<uint32_t, std::chrono::steady_clock::time_point> send_times;

    std::unordered_set<uint32_t> acked_packets;
    std::unordered_map<uint32_t, int> retry_count;
    const int MAX_RETRIES = 3;
    std::unordered_set<uint32_t> failed_packets;
};

#endif