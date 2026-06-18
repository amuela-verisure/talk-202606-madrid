#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <array>
#include <cstdint>
#include <string_view>

#include "demo05/classic/crc32.hpp"
#include "demo05/modern/crc32.hpp"

namespace {

std::uint32_t crc_of(std::string_view text, bool classic) {
    std::array<std::uint8_t, 64> buf{};
    for (std::size_t i = 0; i < text.size() && i < buf.size(); ++i) {
        buf[i] = static_cast<std::uint8_t>(text[i]);
    }
    const std::span<const std::uint8_t> data(buf.data(), text.size());
    return classic ? demo05::classic::crc32_compute(data)
                   : demo05::modern::crc32_compute(data);
}

} // namespace

TEST_CASE("demo05: classic and modern CRC32 agree") {
    demo05::classic::crc32_init();
    for (const auto* text : {"", "a", "abc", "123456789", "embedded c++20"}) {
        CAPTURE(text);
        CHECK(crc_of(text, true) == crc_of(text, false));
    }
}

TEST_CASE("demo05: CRC32 reference value (IEEE 802.3 check)") {
    // crc32("123456789") == 0xCBF43926 is the canonical check value.
    CHECK(crc_of("123456789", false) == 0xCBF43926u);
}

TEST_CASE("demo05: modern CRC is usable at compile time") {
    constexpr std::array<std::uint8_t, 3> abc{0x61u, 0x62u, 0x63u};
    constexpr auto crc = demo05::modern::crc32_compute(abc);
    static_assert(crc == 0x352441C2u); // crc32("abc"), checked by the compiler
    CHECK(crc == 0x352441C2u);
}
