#pragma once
// Classic serialization: memcpy + runtime asserts. The layout contract is
// checked when the code RUNS (and only in debug builds) — a wrong struct
// layout ships silently in release.
#include <cstddef>
#include <cstdint>
#include <span>

namespace demo06::classic {

struct SensorPacket {
    std::uint8_t version;
    std::uint8_t sensor_id;
    std::int16_t reading;
    std::uint32_t timestamp;
};

inline constexpr std::size_t packet_size = 8;

// memcpy into caller buffer; runtime assert checks sizeof — in the binary,
// debug only, too late anyway.
void serialize(const SensorPacket& pkt, std::span<std::uint8_t> out);

// Returns false on short buffer (checked at runtime).
[[nodiscard]] bool deserialize(std::span<const std::uint8_t> in, SensorPacket& out);

// Type punning via memcpy — correct but unverified until runtime.
[[nodiscard]] std::uint32_t float_to_bits(float value);

} // namespace demo06::classic
