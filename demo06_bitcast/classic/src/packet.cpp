#include "demo06/classic/packet.hpp"

#include <cassert>
#include <cstring>

namespace demo06::classic {

void serialize(const SensorPacket& pkt, std::span<std::uint8_t> out) {
    assert(sizeof(SensorPacket) == packet_size); // runtime, debug-only, in .text
    assert(out.size() >= packet_size);
    std::memcpy(out.data(), &pkt, packet_size);
}

bool deserialize(std::span<const std::uint8_t> in, SensorPacket& out) {
    assert(sizeof(SensorPacket) == packet_size);
    if (in.size() < packet_size) {
        return false;
    }
    std::memcpy(&out, in.data(), packet_size);
    return true;
}

std::uint32_t float_to_bits(float value) {
    std::uint32_t bits = 0;
    static_assert(sizeof(bits) == sizeof(value));
    std::memcpy(&bits, &value, sizeof(bits)); // runtime copy the optimizer
    return bits;                              // must see through
}

} // namespace demo06::classic
