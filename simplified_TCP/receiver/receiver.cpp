#include "receiver.h"
#include <iostream>

Receiver::Receiver(UnreliableChannel& ch) : channel(ch), expected_seq_num(0) {}

void Receiver::process_packets() {
    Packet pkt;
    while(channel.recv_from_sender(pkt)){
        if(pkt.seq_num < expected_seq_num){
            std::cout << "[Receiver] Duplicate packet seq=" << pkt.seq_num << ", ignoring. \n";
            continue;
        }

        if (pkt.seq_num == expected_seq_num){
            // its in order, good to send
            deliver_data(&pkt);
            expected_seq_num++;

            // check out_of_order_buffer just in case
            while (out_of_order_buffer.count(expected_seq_num)){
                deliver_data(&out_of_order_buffer[expected_seq_num]);
                out_of_order_buffer.erase(expected_seq_num);
                expected_seq_num++;
            }

        } else if (pkt.seq_num > expected_seq_num){
            // this is out of order. save in the out_of_order_buffer
            std::cout << "[Receiver] Out of order. Buffering packet seq=" <<pkt.seq_num << "\n";
            out_of_order_buffer[pkt.seq_num] = pkt;
        } else{
            std::cout << "[Receiver] Duplicate packet. Already received seq=" << pkt.seq_num << "\n";
        }
        // construct ACK
        Packet ack_pkt(0, pkt.seq_num, true, "");
        channel.send_to_sender(ack_pkt);
        std::cout << "[Receiver] Sent ACK for seq=" << pkt.seq_num << "\n";
    }
}

void Receiver::deliver_data(const Packet* pkt){
    std::cout << "[Receiver] >> Delivered in-order: seq=" << pkt->seq_num << ", data=\"" << pkt->data << "\"\n";
}