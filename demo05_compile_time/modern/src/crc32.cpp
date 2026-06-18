#include "demo05/modern/crc32.hpp"

namespace demo05::modern {

constinit CrcStats stats{0u, 0u};

std::uint32_t crc32_checked(std::span<const std::uint8_t> data) {
    const std::uint32_t crc = crc32_compute(data);
    ++stats.frames_checked;
    stats.last_crc = crc;
    return crc;
}

} // namespace demo05::modern
