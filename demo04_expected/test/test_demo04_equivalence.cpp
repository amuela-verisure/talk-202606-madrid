#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <array>
#include <cstdint>
#include <vector>

#include "demo04/classic/telemetry.hpp"
#include "demo04/modern/telemetry.hpp"

namespace {

std::uint8_t checksum_of(std::span<const std::uint8_t> bytes) {
    std::uint8_t sum = 0;
    for (std::uint8_t b : bytes) {
        sum = static_cast<std::uint8_t>(sum + b);
    }
    return sum;
}

std::array<std::uint8_t, 5> make_frame(std::uint8_t id, std::int16_t reading) {
    std::array<std::uint8_t, 5> f{};
    f[0] = 0xA5;
    f[1] = id;
    f[2] = static_cast<std::uint8_t>(static_cast<std::uint16_t>(reading) & 0xFFu);
    f[3] = static_cast<std::uint8_t>(static_cast<std::uint16_t>(reading) >> 8u);
    f[4] = checksum_of(std::span<const std::uint8_t>(f.data(), 4));
    return f;
}

} // namespace

TEST_CASE("demo04: valid frame parses identically") {
    const auto frame = make_frame(7, 250);

    const auto classic_frame = demo04::classic::parse_frame(frame);
    const auto modern_frame = demo04::modern::parse_frame(frame);

    REQUIRE(modern_frame.has_value());
    CHECK(classic_frame.sensor_id == modern_frame->sensor_id);
    CHECK(classic_frame.reading == modern_frame->reading);
}

TEST_CASE("demo04: both reject the same malformed frames") {
    auto bad_sync = make_frame(1, 100);
    bad_sync[0] = 0x5A;

    auto bad_checksum = make_frame(2, 100);
    bad_checksum[4] = static_cast<std::uint8_t>(bad_checksum[4] + 1u);

    const auto out_of_range = make_frame(3, 2000);

    CHECK_THROWS_AS((void)demo04::classic::parse_frame(bad_sync),
                    demo04::classic::BadSync);
    CHECK_THROWS_AS((void)demo04::classic::parse_frame(bad_checksum),
                    demo04::classic::BadChecksum);
    CHECK_THROWS_AS((void)demo04::classic::parse_frame(out_of_range),
                    demo04::classic::OutOfRange);

    CHECK(demo04::modern::parse_frame(bad_sync).error() ==
          demo04::modern::FrameError::BadSync);
    CHECK(demo04::modern::parse_frame(bad_checksum).error() ==
          demo04::modern::FrameError::BadChecksum);
    CHECK(demo04::modern::parse_frame(out_of_range).error() ==
          demo04::modern::FrameError::OutOfRange);
}

TEST_CASE("demo04: stream statistics are identical") {
    std::vector<std::uint8_t> stream;
    for (int i = 0; i < 50; ++i) {
        auto frame = make_frame(static_cast<std::uint8_t>(i),
                                static_cast<std::int16_t>(i * 37 - 600));
        if (i % 5 == 0) {
            frame[4] = static_cast<std::uint8_t>(frame[4] ^ 0xFFu); // corrupt
        }
        if (i % 11 == 0) {
            frame[0] = 0x00; // lose sync
        }
        stream.insert(stream.end(), frame.begin(), frame.end());
    }

    const auto classic_stats = demo04::classic::process_stream(stream);
    const auto modern_stats = demo04::modern::process_stream(stream);

    CHECK(classic_stats.accepted == modern_stats.accepted);
    CHECK(classic_stats.rejected == modern_stats.rejected);
    CHECK(classic_stats.sum == modern_stats.sum);
}
