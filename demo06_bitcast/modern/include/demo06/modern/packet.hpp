#pragma once
// Modern serialization: std::bit_cast + static_assert. The layout contract
// is enforced by the COMPILER — size, offsets, triviality and endianness are
// all checked before the binary exists. Zero bytes of checking code ship.
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <type_traits>

namespace demo06::modern {

struct SensorPacket {
    std::uint8_t version;
    std::uint8_t sensor_id;
    std::int16_t reading;
    std::uint32_t timestamp;
};

inline constexpr std::size_t packet_size = 8;

// The wire contract, compiler-verified. Wrong layout = no binary.
static_assert(sizeof(SensorPacket) == packet_size);
static_assert(offsetof(SensorPacket, version) == 0);
static_assert(offsetof(SensorPacket, sensor_id) == 1);
static_assert(offsetof(SensorPacket, reading) == 2);
static_assert(offsetof(SensorPacket, timestamp) == 4);
static_assert(std::is_trivially_copyable_v<SensorPacket>);
static_assert(std::endian::native == std::endian::little,
              "wire format assumes little-endian host");

using PacketBytes = std::array<std::uint8_t, packet_size>;

// std::bit_cast: UB-free, size-checked at compile time, constexpr-capable.
[[nodiscard]] constexpr PacketBytes serialize(const SensorPacket& pkt) {
    return std::bit_cast<PacketBytes>(pkt);
}

[[nodiscard]] std::optional<SensorPacket>
deserialize(std::span<const std::uint8_t> in);

// Type punning as a constant expression — try that with memcpy or a union.
[[nodiscard]] constexpr std::uint32_t float_to_bits(float value) {
    return std::bit_cast<std::uint32_t>(value);
}
static_assert(float_to_bits(1.0f) == 0x3F800000u); // IEEE-754, compiler-checked

// Out-of-line wrappers for size comparison with the classic version.
void serialize_to(const SensorPacket& pkt, std::span<std::uint8_t> out);
[[nodiscard]] std::uint32_t float_bits(float value);

} // namespace demo06::modern
