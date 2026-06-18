#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <memory>

#include "demo03/classic/particles.hpp"
#include "demo03/classic/particles_ptr.hpp"
#include "demo03/modern/particles.hpp"

TEST_CASE("demo03: all three storage stages produce identical trajectories") {
    constexpr std::size_t count = 1024;
    constexpr float dt = 0.016f;
    constexpr int steps = 100;

    auto ptr_particles = demo03::classic::make_particles_ptr(count);
    auto aos_particles = demo03::classic::make_particles(count);
    auto soa_particles = std::make_unique<demo03::modern::Particles>();
    demo03::modern::make_particles(*soa_particles, count);

    for (int s = 0; s < steps; ++s) {
        demo03::classic::update(ptr_particles, dt);
        demo03::classic::update(aos_particles, dt);
        demo03::modern::update(*soa_particles, dt);
    }

    for (std::size_t i = 0; i < count; ++i) {
        CAPTURE(i);
        REQUIRE(ptr_particles[i]->pos_x() == aos_particles[i].x);
        REQUIRE(ptr_particles[i]->pos_y() == aos_particles[i].y);
        REQUIRE(aos_particles[i].x == soa_particles->x[i]);
        REQUIRE(aos_particles[i].y == soa_particles->y[i]);
    }
}

TEST_CASE("demo03: alive counts match across all stages") {
    constexpr std::size_t count = 333;
    auto ptr_particles = demo03::classic::make_particles_ptr(count);
    auto aos_particles = demo03::classic::make_particles(count);
    auto soa_particles = std::make_unique<demo03::modern::Particles>();
    demo03::modern::make_particles(*soa_particles, count);

    const auto expected = demo03::classic::count_alive(aos_particles);
    CHECK(demo03::classic::count_alive(ptr_particles) == expected);
    CHECK(demo03::modern::count_alive(*soa_particles) == expected);
}
