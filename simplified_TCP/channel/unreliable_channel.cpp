#include "unreliable_channel.h"
#include <random>
#include <chrono>
#include <thread>
#include <iostream>

UnreliableChannel::UnreliableChannel(double packet_loss, double ack_loss, int delay_ms)
    : packet_loss_rate(packet_loss), ack_loss_rate(ack_loss), propagation_delay_ms(delay_ms) {}

void UnreliableChannel::send_to_receiver(const Packet& pkt) {
    // Simulate packet loss
    if ((double)rand() / RAND_MAX < packet_loss_rate) {
        std::cout << "[Channel] Dropped packet: " << pkt.seq_num << "\n";
        return;
    }

    std::thread(&UnreliableChannel::deliver_to_receiver, this, pkt).detach();
}

void UnreliableChannel::send_to_sender(const Packet& ack) {
    if ((double)rand() / RAND_MAX < ack_loss_rate) {
        std::cout << "[Channel] Dropped ACK: " << ack.ack_num << "\n";
        return;
    }

    std::thread(&UnreliableChannel::deliver_to_sender, this, ack).detach();
}

void UnreliableChannel::deliver_to_receiver(Packet pkt) {
    std::this_thread::sleep_for(std::chrono::milliseconds(propagation_delay_ms));

    std::lock_guard<std::mutex> lock(mtx);
    sender_to_receiver.push(pkt);
    cv.notify_all();
}

void UnreliableChannel::deliver_to_sender(Packet ack) {
    std::this_thread::sleep_for(std::chrono::milliseconds(propagation_delay_ms));

    std::lock_guard<std::mutex> lock(mtx);
    receiver_to_sender.push(ack);
    cv.notify_all();
}

bool UnreliableChannel::recv_from_sender(Packet& out_pkt) {
    std::unique_lock<std::mutex> lock(mtx);
    if (sender_to_receiver.empty()) return false;

    out_pkt = sender_to_receiver.front();
    sender_to_receiver.pop();
    return true;
}

bool UnreliableChannel::recv_from_receiver(Packet& out_ack) {
    std::unique_lock<std::mutex> lock(mtx);
    if (receiver_to_sender.empty()) return false;

    out_ack = receiver_to_sender.front();
    receiver_to_sender.pop();
    return true;
}
