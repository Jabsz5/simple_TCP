#include "sender.h"
#include <iostream>

Sender::Sender(UnreliableChannel& ch, int win_size, int timeout)
    : channel(ch), window_size(win_size), timeout(timeout),
    next_seq_num(0), base(0) {}

void Sender::send_data(const std::vector<std::string>& messages) {
    while (next_seq_num < messages.size()) {
        // check if window is full here
        if (next_seq_num >= base + window_size ){
            std::cout << "[Sender] Window is full. Waiting for ACKs...\n";
            break;
        }


        // let's create the packet and send it here
        Packet p(next_seq_num, 0, false, messages[next_seq_num]);
        channel.send_to_receiver(p);
        in_flight[next_seq_num] = p;
        send_times[next_seq_num] = std::chrono::steady_clock::now();

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
            send_times.erase(ack_num);

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

void Sender::check_timeouts() {
    auto now = std::chrono::steady_clock::now();

    for (auto it = in_flight.begin(); it != in_flight.end(); ) {
        uint32_t seq = it->first;
        Packet& pkt = it->second;

        if (send_times.find(seq) == send_times.end()) {
            ++it;
            continue;
        }

        auto time_sent = send_times[seq];
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - time_sent).count() > timeout) {
            retry_count[seq]++;

            if (retry_count[seq] > MAX_RETRIES) {
                std::cout << "[Sender] Max retries exceeded for seq=" << seq << ". Giving up.\n";
                failed_packets.insert(seq);

                send_times.erase(seq);
                it = in_flight.erase(it);  // SAFE: erase and advance
                // Slide base if this was the base
                if (seq == base) {
                    while (!in_flight.count(base) && base < next_seq_num) {
                        base++;
                    }
                }
                continue;
            }

            std::cout << "[Sender] Timeout! Retransmitting seq=" << seq
                      << " (attempt " << retry_count[seq] << ")\n";
            channel.send_to_receiver(pkt);
            send_times[seq] = now;
        }

        ++it;  // safe increment
    }
}


bool Sender::is_done(size_t total_packets){
    return base >= total_packets;
}