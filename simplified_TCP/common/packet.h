#ifndef PACKET_H
#define PACKET_H

#include <string>
#include <cstdint>


struct Packet {
    uint32_t seq_num;
    uint32_t ack_num;
    bool is_ack;

    std::string data;

    Packet()
        : seq_num(0), ack_num(0), is_ack(false), data("") {}

    Packet(uint32_t seq, uint32_t ack, bool ack_flag, const std::string& payload)
        : seq_num(seq), ack_num(ack), is_ack(ack_flag), data(payload) {}

    // Serialize to string (for channel simulation)
    std::string serialize() const {
        return std::to_string(seq_num) + "|" + 
               std::to_string(ack_num) + "|" +
               (is_ack ? "1" : "0") + "|" +
               data;
    }

    // Deserialize from string (for channel simulation)
    static Packet deserialize(const std::string& s) {
        Packet p;
        size_t pos1 = s.find('|');
        size_t pos2 = s.find('|', pos1 + 1);
        size_t pos3 = s.find('|', pos2 + 1);

        p.seq_num = std::stoul(s.substr(0, pos1));
        p.ack_num = std::stoul(s.substr(pos1 + 1, pos2 - pos1 - 1));
        p.is_ack = s[pos2 + 1] == '1';
        p.data = s.substr(pos3 + 1);

        return p;
    }


};

#endif 