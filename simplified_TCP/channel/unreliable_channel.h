#ifndef UNRELIABLE_CHANNEL_H
#define UNRELIABLE_CHANNEL_H

#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "../common/packet.h"

class UnreliableChannel {
public:
    UnreliableChannel(double packet_loss, double ack_loss, int delay_ms);

    // Called by sender
    void send_to_receiver(const Packet& pkt);

    // Called by receiver
    void send_to_sender(const Packet& ack);

    // Called by receiver to get next data packet
    bool recv_from_sender(Packet& out_pkt);

    // Called by sender to get next ACK
    bool recv_from_receiver(Packet& out_ack);

private:
    double packet_loss_rate;
    double ack_loss_rate;
    int propagation_delay_ms;

    std::queue<Packet> sender_to_receiver;
    std::queue<Packet> receiver_to_sender;

    std::mutex mtx;
    std::condition_variable cv;

    // Background delivery workers
    void deliver_to_receiver(Packet pkt);
    void deliver_to_sender(Packet ack);
};

#endif