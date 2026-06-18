#include "demo04/modern/telemetry.hpp"

namespace demo04::modern {

namespace {

std::uint8_t checksum(std::span<const std::uint8_t> bytes) {
    std::uint8_t sum = 0;
    for (std::uint8_t b : bytes) {
        sum = static_cast<std::uint8_t>(sum + b);
    }
    return sum;
}

tl::expected<std::span<const std::uint8_t>, FrameError>
check_length(std::span<const std::uint8_t> bytes) {
    if (bytes.size() != frame_size) {
        return tl::unexpected{FrameError::BadLength};
    }
    return bytes;
}

tl::expected<std::span<const std::uint8_t>, FrameError>
check_sync(std::span<const std::uint8_t> bytes) {
    if (bytes[0] != sync_byte) {
        return tl::unexpected{FrameError::BadSync};
    }
    return bytes;
}

tl::expected<std::span<const std::uint8_t>, FrameError>
check_checksum(std::span<const std::uint8_t> bytes) {
    if (checksum(bytes.first(frame_size - 1)) != bytes[frame_size - 1]) {
        return tl::unexpected{FrameError::BadChecksum};
    }
    return bytes;
}

tl::expected<Frame, FrameError> decode(std::span<const std::uint8_t> bytes) {
    const auto reading = static_cast<std::int16_t>(
        static_cast<std::uint16_t>(bytes[2]) |
        static_cast<std::uint16_t>(static_cast<std::uint16_t>(bytes[3]) << 8u));
    if (reading < -1000 || reading > 1000) {
        return tl::unexpected{FrameError::OutOfRange};
    }
    return Frame{bytes[1], reading};
}

} // namespace

tl::expected<Frame, FrameError> parse_frame(std::span<const std::uint8_t> bytes) {
    // Monadic chain: reads top to bottom, CCN 1, no nesting, no unwinding.
    return check_length(bytes)
        .and_then(check_sync)
        .and_then(check_checksum)
        .and_then(decode);
}

StreamStats process_stream(std::span<const std::uint8_t> bytes) {
    StreamStats stats{};
    for (std::size_t offset = 0; offset + frame_size <= bytes.size();
         offset += frame_size) {
        parse_frame(bytes.subspan(offset, frame_size))
            .map([&](const Frame& frame) {
                ++stats.accepted;
                stats.sum += frame.reading;
            })
            .map_error([&](FrameError) { ++stats.rejected; });
    }
    return stats;
}

} // namespace demo04::modern
