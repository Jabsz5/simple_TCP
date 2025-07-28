#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

#include "common/packet.h"
#include "channel/unreliable_channel.h"
#include "sender/sender.h"
#include "receiver/receiver.h"

int main() {
    double packet_loss, ack_loss;
    int propagation_delay_ms, window_size;
    srand(time(nullptr));

    // channel(packet loss rate, ACK loss rate, propagation delay)

    std::cout << "Enter packet loss rate: ";
    std::cin >> packet_loss;
    std::cout << "\nEnter ACK loss rate: ";
    std::cin >> ack_loss;
    std::cout << "\nEnter propagation delay (ms): ";
    std::cin >> propagation_delay_ms;
    UnreliableChannel channel(packet_loss, ack_loss, propagation_delay_ms);

    // create a sender and a receiver
    std::cout <<"Enter window size for sender: ";
    std::cin >> window_size;

    int timeout_ms = 2 * propagation_delay_ms + 300 + 200; // safety

    Sender sender(channel, window_size, timeout_ms);
    Receiver receiver(channel);

    std::vector<std::string> messages;
    for (int i = 0; i < 20; i++){
        messages.push_back("Packet " + std::to_string(i));
    }

    std::cout << "\n=== Starting TCP Simulation ===\n";

    while (!sender.is_done(messages.size())){
        sender.send_data(messages);
        sender.check_timeouts();
        std::cout << "\n";

        receiver.process_packets();
        std::cout << "\n";

        sender.receive_acks();
        std::cout << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    
    const auto& failed = sender.get_failed_packets();
    if (!failed.empty()){
        std::cout << "[Logger] Failed to deliver the following packets after max retries: \n";
        for (auto seq : failed){
            std::cout << " - Packet seq=" << seq << '\n';
        }
    } else{
        std::cout << "[Logger] All packets were successfully delivered and acknowledged.\n";
    }
    
    
    return 0;
}
