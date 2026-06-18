#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <array>
#include <cstdint>

#include "demo06/classic/packet.hpp"
#include "demo06/modern/packet.hpp"

TEST_CASE("demo06: serialization produces identical bytes") {
    const demo06::classic::SensorPacket classic_pkt{2, 14, -512, 0xDEADBEEFu};
    const demo06::modern::SensorPacket modern_pkt{2, 14, -512, 0xDEADBEEFu};

    std::array<std::uint8_t, 8> classic_bytes{};
    demo06::classic::serialize(classic_pkt, classic_bytes);
    const auto modern_bytes = demo06::modern::serialize(modern_pkt);

    for (std::size_t i = 0; i < modern_bytes.size(); ++i) {
        CAPTURE(i);
        CHECK(classic_bytes[i] == modern_bytes[i]);
    }
}

TEST_CASE("demo06: deserialization round-trips identically") {
    const demo06::modern::SensorPacket original{1, 3, 999, 123456u};
    const auto bytes = demo06::modern::serialize(original);

    demo06::classic::SensorPacket classic_out{};
    REQUIRE(demo06::classic::deserialize(bytes, classic_out));
    const auto modern_out = demo06::modern::deserialize(bytes);
    REQUIRE(modern_out.has_value());

    CHECK(classic_out.version == modern_out->version);
    CHECK(classic_out.sensor_id == modern_out->sensor_id);
    CHECK(classic_out.reading == modern_out->reading);
    CHECK(classic_out.timestamp == modern_out->timestamp);
}

TEST_CASE("demo06: float bit patterns agree (and modern is constexpr)") {
    for (float f : {0.0f, 1.0f, -1.0f, 3.14159f, 1e-38f}) {
        CAPTURE(f);
        CHECK(demo06::classic::float_to_bits(f) ==
              demo06::modern::float_to_bits(f));
    }
    static_assert(demo06::modern::float_to_bits(-2.0f) == 0xC0000000u);
}

TEST_CASE("demo06: short buffers are rejected by both") {
    std::array<std::uint8_t, 4> short_buf{};
    demo06::classic::SensorPacket classic_out{};
    CHECK_FALSE(demo06::classic::deserialize(short_buf, classic_out));
    CHECK_FALSE(demo06::modern::deserialize(short_buf).has_value());
}
