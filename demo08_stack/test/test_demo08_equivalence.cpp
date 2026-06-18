#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "demo08/classic/event_log.hpp"
#include "demo08/modern/event_log.hpp"

TEST_CASE("demo08: reserve removes reallocation but not the heap or the "
          "silent overflow") {
    demo08::classic::EventLog reserved_log{demo08::modern::log_capacity};
    const auto initial_capacity = reserved_log.capacity();
    CHECK(initial_capacity >= demo08::modern::log_capacity);

    for (std::uint32_t i = 0; i < demo08::modern::log_capacity; ++i) {
        reserved_log.record({i, 0, 0});
    }
    // No reallocation while within the reserved budget...
    CHECK(reserved_log.capacity() == initial_capacity);

    // ...but one event past the budget silently grows the vector instead of
    // reporting failure — the budget is not part of the contract.
    reserved_log.record({999u, 0, 0});
    CHECK(reserved_log.size() == demo08::modern::log_capacity + 1);
    CHECK(reserved_log.capacity() > initial_capacity);
}

TEST_CASE("demo08: logs agree while within the capacity budget") {
    demo08::classic::EventLog classic_log;
    demo08::modern::EventLog modern_log;

    for (std::uint32_t i = 0; i < demo08::modern::log_capacity; ++i) {
        const auto code = static_cast<std::uint16_t>(i % 7u);
        const auto source = static_cast<std::uint16_t>(i % 3u);
        classic_log.record({i * 10u, code, source});
        CHECK(modern_log.record({i * 10u, code, source}));
    }

    REQUIRE(classic_log.size() == modern_log.size());
    CHECK(classic_log.checksum() == modern_log.checksum());
    for (std::size_t i = 0; i < modern_log.size(); ++i) {
        CAPTURE(i);
        CHECK(classic_log[i].timestamp == modern_log[i].timestamp);
        CHECK(classic_log[i].code == modern_log[i].code);
    }
}

TEST_CASE("demo08: overflow is explicit in the modern log") {
    demo08::modern::EventLog modern_log;
    for (std::uint32_t i = 0; i < demo08::modern::log_capacity; ++i) {
        CHECK(modern_log.record({i, 0, 0}));
    }
    // Capacity 65th event: classic vector would silently realloc (or throw
    // bad_alloc); the modern log reports failure as a value.
    CHECK_FALSE(modern_log.record({999u, 0, 0}));
    CHECK(modern_log.size() == demo08::modern::log_capacity);
}

TEST_CASE("demo08: clear resets both logs the same way") {
    demo08::classic::EventLog classic_log;
    demo08::modern::EventLog modern_log;
    classic_log.record({1u, 2, 3});
    CHECK(modern_log.record({1u, 2, 3}));
    classic_log.clear();
    modern_log.clear();
    CHECK(classic_log.size() == modern_log.size());
    CHECK(classic_log.checksum() == modern_log.checksum());
}
