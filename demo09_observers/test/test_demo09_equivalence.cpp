#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <array>
#include <cstdint>

#include "demo09/classic/subject.hpp"
#include "demo09/modern/subject.hpp"

namespace {

struct Tally {
    std::uint32_t count{};
    std::int64_t sum{};
};

} // namespace

TEST_CASE("demo09: both observer schemes deliver identical notifications") {
    Tally classic_alarm{}, classic_stats{};
    Tally modern_alarm{}, modern_stats{};

    demo09::classic::Subject classic_subject;
    classic_subject.subscribe([&](const demo09::classic::Event& e) {
        if (e.code == 0xFFu) {
            ++classic_alarm.count;
            classic_alarm.sum += e.value;
        }
    });
    classic_subject.subscribe([&](const demo09::classic::Event& e) {
        ++classic_stats.count;
        classic_stats.sum += e.value;
    });

    demo09::modern::Subject modern_subject{
        [&](const demo09::modern::Event& e) {
            if (e.code == 0xFFu) {
                ++modern_alarm.count;
                modern_alarm.sum += e.value;
            }
        },
        [&](const demo09::modern::Event& e) {
            ++modern_stats.count;
            modern_stats.sum += e.value;
        },
    };

    CHECK(classic_subject.observer_count() == modern_subject.observer_count());

    constexpr std::array<std::pair<std::uint16_t, std::int32_t>, 5> events = {
        {{0x01u, 10}, {0xFFu, -3}, {0x02u, 7}, {0xFFu, 42}, {0x03u, 0}}};

    for (const auto& [code, value] : events) {
        classic_subject.notify({code, value});
        modern_subject.notify({code, value});
    }

    CHECK(classic_alarm.count == modern_alarm.count);
    CHECK(classic_alarm.sum == modern_alarm.sum);
    CHECK(classic_stats.count == modern_stats.count);
    CHECK(classic_stats.sum == modern_stats.sum);
}

TEST_CASE("demo09: modern observer count is a compile-time constant") {
    auto noop = [](const demo09::modern::Event&) {};
    demo09::modern::Subject subject{noop};
    static_assert(decltype(subject)::observer_count() == 1);
    CHECK(subject.observer_count() == 1);
}
