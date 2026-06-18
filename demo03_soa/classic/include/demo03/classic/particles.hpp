#pragma once
// Stage 2 — Array-of-Structs (AoS): drop the base class, store VALUES in one
// contiguous vector. Already a big win over stage 1 (particles_ptr.hpp): one
// allocation instead of N, no vtable, no pointer chasing. Remaining cost:
// each update drags cold bytes (id, flags, padding) through the cache to
// reach the four floats it needs.
#include <cstddef>
#include <cstdint>
#include <vector>

namespace demo03::classic {

struct Particle {
    float x{};
    float y{};
    float vx{};
    float vy{};
    std::uint32_t id{};
    bool alive{true};
    // 3 bytes of padding — paid on every cache line, used never.
};

using Particles = std::vector<Particle>;

[[nodiscard]] Particles make_particles(std::size_t count);

// Integrate positions for the alive particles.
void update(Particles& particles, float dt);

[[nodiscard]] std::size_t count_alive(const Particles& particles);

} // namespace demo03::classic
