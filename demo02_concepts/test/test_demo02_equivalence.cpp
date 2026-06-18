#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <array>

#include "demo02/classic/filters.hpp"
#include "demo02/modern/filters.hpp"

namespace {
constexpr std::array<float, 8> samples = {1.0f,  5.0f, 3.0f, 8.0f,
                                          -2.0f, 4.0f, 7.0f, 0.5f};
}

TEST_CASE("demo02: low-pass filters produce identical output") {
    demo02::classic::LowPass classic_filter{0.3f};
    demo02::modern::LowPass modern_filter{.alpha = 0.3f};

    CHECK(demo02::classic::run_filter(classic_filter, samples) ==
          demo02::modern::run_lowpass(modern_filter, samples));
}

TEST_CASE("demo02: high-pass filters produce identical output") {
    demo02::classic::HighPass classic_filter{0.7f};
    demo02::modern::HighPass modern_filter{.alpha = 0.7f};

    CHECK(demo02::classic::run_filter(classic_filter, samples) ==
          demo02::modern::run_highpass(modern_filter, samples));
}

TEST_CASE("demo02: modern filter works in constant expressions") {
    constexpr float result = [] {
        demo02::modern::LowPass f{.alpha = 0.5f};
        constexpr std::array<float, 3> data = {2.0f, 2.0f, 2.0f};
        return demo02::modern::run_filter(f, data);
    }();
    static_assert(result > 0.0f);
    CHECK(result == doctest::Approx(1.75f));
}
