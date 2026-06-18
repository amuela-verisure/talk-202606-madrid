#include "demo05/classic/crc32.hpp"

namespace demo05::classic {

namespace {
std::uint32_t crc_table[256]; // .bss — RAM, garbage until crc32_init() runs
bool table_ready = false;     // lazy-init guard, checked on every call
} // namespace

void crc32_init() {
    for (std::uint32_t i = 0; i < 256u; ++i) {
        std::uint32_t crc = i;
        for (int j = 0; j < 8; ++j) {
            crc = (crc & 1u) != 0u ? (crc >> 1u) ^ 0xEDB88320u : crc >> 1u;
        }
        crc_table[i] = crc;
    }
    table_ready = true;
}

std::uint32_t crc32_compute(std::span<const std::uint8_t> data) {
    if (!table_ready) { // runtime branch, every single call
        crc32_init();
    }
    std::uint32_t crc = 0xFFFFFFFFu;
    for (std::uint8_t byte : data) {
        crc = (crc >> 8u) ^ crc_table[(crc ^ byte) & 0xFFu];
    }
    return crc ^ 0xFFFFFFFFu;
}

} // namespace demo05::classic
