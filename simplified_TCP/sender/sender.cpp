#include "sender.h"
#include <iostream>

Sender::Sender(UnreliableChannel& ch, int win_size)
    : channel(ch), window_size(win_size), next_seq_num(0), base(0) {}

void Sender::send_data(const std::vector<std::string>& messages) {
    while (next_seq_num < messages.size()) {
        // check if window is full here
        if (next_seq_num >= base + window_size){
            std::cout << "[Sender] Window is full. Waiting for ACKs...\n";
            break;
        }


        // let's create the packet and send it here
        Packet p(next_seq_num, 0, false, messages[next_seq_num]);
        channel.send_to_receiver(p);
        in_flight[next_seq_num] = p;

        std::cout << "[Sender] Sent packet seq=" << next_seq_num << ", data=\"" << p.data << "\"\n";
        //std::cout << "[Sender Debug] base=" << base
        //  << ", next_seq_num=" << next_seq_num
        //  << ", window max=" << base + window_size << std::endl;

        next_seq_num++;
    }
}

void Sender::receive_acks(){
    Packet ack_pkt;

    while(channel.recv_from_receiver(ack_pkt)) {
        if (!ack_pkt.is_ack) continue; // ignore if already ACK'd

        uint32_t ack_num = ack_pkt.ack_num;

        std::cout << "[Sender] Received ACK for seq= " << ack_num << std::endl;

        if (ack_num >= base && ack_num < next_seq_num) {
            in_flight.erase(ack_num); // remove from unACK'd buffer space 

            // slide base if possible
            if (ack_num == base){
                // slide base to the front of all ACK'd packets
                while (!in_flight.count(base) && base < next_seq_num){
                    base++;
                }
            }
        }
    }
}