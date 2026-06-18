#pragma once
// Classic error handling: exceptions used as CONTROL FLOW. Malformed frames
// are an expected, frequent input — yet every rejection unwinds the stack.
// Requires -fexceptions, the unwinder runtime, and exception tables in flash.
#include <cstdint>
#include <span>
#include <stdexcept>

namespace demo04::classic {

struct BadSync final : std::runtime_error {
    BadSync() : std::runtime_error("bad sync byte") {}
};

struct BadLength final : std::runtime_error {
    BadLength() : std::runtime_error("bad frame length") {}
};

struct BadChecksum final : std::runtime_error {
    BadChecksum() : std::runtime_error("checksum mismatch") {}
};

struct OutOfRange final : std::runtime_error {
    OutOfRange() : std::runtime_error("reading out of range") {}
};

struct Frame {
    std::uint8_t sensor_id{};
    std::int16_t reading{};
};

struct StreamStats {
    std::uint32_t accepted{};
    std::uint32_t rejected{};
    std::int32_t sum{};
};

// Frame layout: [0xA5][sensor_id][reading_lo][reading_hi][checksum]
inline constexpr std::size_t frame_size = 5;
inline constexpr std::uint8_t sync_byte = 0xA5;

// Throws BadSync / BadLength / BadChecksum / OutOfRange.
[[nodiscard]] Frame parse_frame(std::span<const std::uint8_t> bytes);

// try/catch per frame: the "expected error" path is a stack unwind.
[[nodiscard]] StreamStats process_stream(std::span<const std::uint8_t> bytes);

} // namespace demo04::classic
