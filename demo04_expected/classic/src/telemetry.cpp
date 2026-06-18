#include "demo04/classic/telemetry.hpp"

namespace demo04::classic {

namespace {

std::uint8_t checksum(std::span<const std::uint8_t> bytes) {
    std::uint8_t sum = 0;
    for (std::uint8_t b : bytes) {
        sum = static_cast<std::uint8_t>(sum + b);
    }
    return sum;
}

} // namespace

Frame parse_frame(std::span<const std::uint8_t> bytes) {
    if (bytes.size() != frame_size) {
        throw BadLength{};
    }
    if (bytes[0] != sync_byte) {
        throw BadSync{};
    }
    if (checksum(bytes.first(frame_size - 1)) != bytes[frame_size - 1]) {
        throw BadChecksum{};
    }
    const auto reading = static_cast<std::int16_t>(
        static_cast<std::uint16_t>(bytes[2]) |
        static_cast<std::uint16_t>(static_cast<std::uint16_t>(bytes[3]) << 8u));
    if (reading < -1000 || reading > 1000) {
        throw OutOfRange{};
    }
    return Frame{bytes[1], reading};
}

StreamStats process_stream(std::span<const std::uint8_t> bytes) {
    StreamStats stats{};
    for (std::size_t offset = 0; offset + frame_size <= bytes.size();
         offset += frame_size) {
        try {
            const Frame frame = parse_frame(bytes.subspan(offset, frame_size));
            ++stats.accepted;
            stats.sum += frame.reading;
        } catch (const std::runtime_error&) {
            // Exceptions as control flow: a corrupted frame is NOT exceptional
            // on a radio link — yet every single one costs a stack unwind.
            ++stats.rejected;
        }
    }
    return stats;
}

} // namespace demo04::classic
