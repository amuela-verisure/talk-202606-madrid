#pragma once
// Modern error handling: tl::expected (std::expected in C++23).
// Errors are values: visible in the signature, handled monadically, no
// unwinder, no exception tables, deterministic cost on both paths.
// Compiles with -fno-exceptions.
#include <cstdint>
#include <span>

#include <tl/expected.hpp>

namespace demo04::modern {

enum class FrameError : std::uint8_t {
    BadSync,
    BadLength,
    BadChecksum,
    OutOfRange,
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

// The full set of outcomes is part of the type. [[nodiscard]] on the
// expected makes ignoring an error a warning, not a silent bug.
[[nodiscard]] tl::expected<Frame, FrameError>
parse_frame(std::span<const std::uint8_t> bytes);

[[nodiscard]] StreamStats process_stream(std::span<const std::uint8_t> bytes);

// UPGRADE PATH [C++23]: s/tl::expected/std::expected/ and drop the dependency.

} // namespace demo04::modern
