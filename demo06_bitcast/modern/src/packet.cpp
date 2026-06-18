#include "demo06/modern/packet.hpp"

#include <algorithm>

namespace demo06::modern {

std::optional<SensorPacket> deserialize(std::span<const std::uint8_t> in) {
    if (in.size() < packet_size) {
        return std::nullopt; // the only runtime check left: input length
    }
    PacketBytes bytes{};
    std::copy_n(in.begin(), packet_size, bytes.begin());
    return std::bit_cast<SensorPacket>(bytes);
}

void serialize_to(const SensorPacket& pkt, std::span<std::uint8_t> out) {
    if (out.size() < packet_size) {
        return;
    }
    const PacketBytes bytes = serialize(pkt);
    std::copy_n(bytes.begin(), packet_size, out.begin());
}

std::uint32_t float_bits(float value) { return float_to_bits(value); }

} // namespace demo06::modern
