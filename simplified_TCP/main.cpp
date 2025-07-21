#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include "common/packet.h"
#include "channel/unreliable_channel.h"
#include "sender/sender.h"

int main() {
    srand(time(nullptr));

    // No packet loss, short delay to keep things visible
    UnreliableChannel channel(0.0, 0.0, 200);

    // Sender with window size = 3
    Sender sender(channel, 3);

    // Messages to send
    std::vector<std::string> messages = {
        "Packet 0", "Packet 1", "Packet 2", "Packet 3", "Packet 4"
    };

    std::cout << "\n=== Sending Packets ===\n";
    sender.send_data(messages);

    std::cout << "=== Done Sending (no ACKs yet) ===\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(800));

    // manually simulate for now 
    channel.send_to_sender(Packet(0,0,true, "We've established a connection :D"));
    channel.send_to_sender(Packet(0,1,true, "what up chat?"));
    channel.send_to_sender(Packet(0,2,true, "eggs "));

    std::cout << "\n=== Receiving ACKs ===\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    sender.receive_acks();

    // try to send more 
    std::cout << "\n=== Sending Remaining Packets ===\n";
    sender.send_data(messages);

    return 0;
}
