#pragma once
// Modern compile-time initialization: consteval guarantees the table is
// computed by the compiler — it lives in .rodata (flash), costs zero RAM and
// zero boot cycles, and is verified by static_assert before the code ships.
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace demo05::modern {

// consteval (vs constexpr): this CANNOT silently fall back to runtime.
consteval std::array<std::uint32_t, 256> make_crc32_table() {
    std::array<std::uint32_t, 256> table{};
    for (std::uint32_t i = 0; i < 256u; ++i) {
        std::uint32_t crc = i;
        for (int j = 0; j < 8; ++j) {
            crc = (crc & 1u) != 0u ? (crc >> 1u) ^ 0xEDB88320u : crc >> 1u;
        }
        table[i] = crc;
    }
    return table;
}

// .rodata, no init function, no "is it ready?" flag, immutable by type.
inline constexpr auto crc_table = make_crc32_table();

// The table is verified at COMPILE TIME — a wrong polynomial cannot ship.
static_assert(crc_table[0] == 0x00000000u);
static_assert(crc_table[1] == 0x77073096u);
static_assert(crc_table[255] == 0x2D02EF8Du);

// constinit: mutable runtime state, but its INITIALIZER is compile-time
// checked — static-init-order fiasco ruled out by the keyword itself.
struct CrcStats {
    std::uint32_t frames_checked;
    std::uint32_t last_crc;
};
extern constinit CrcStats stats;

[[nodiscard]] constexpr std::uint32_t
crc32_compute(std::span<const std::uint8_t> data) {
    std::uint32_t crc = 0xFFFFFFFFu;
    for (std::uint8_t byte : data) {
        crc = (crc >> 8u) ^ crc_table[(crc ^ byte) & 0xFFu];
    }
    return crc ^ 0xFFFFFFFFu;
}

// Out-of-line wrapper (updates stats) for size comparison with classic.
[[nodiscard]] std::uint32_t crc32_checked(std::span<const std::uint8_t> data);

} // namespace demo05::modern
