#pragma once
// Stage 3 — Struct-of-Arrays (SoA): one struct holding parallel arrays.
// The further refinement over AoS, where the hot loop justifies it: the
// update streams exactly the floats it needs — dense cache lines,
// vectorizer-friendly, fixed capacity on the stack or in .bss (zero heap).
// See V. Romeo, "More Speed and Simplicity" (CppCon 2025).
#include <array>
#include <cstddef>
#include <cstdint>

namespace demo03::modern {

// Compile-time capacity budget, sized for the target: the Cortex-M4
// reference target has 64 KB of RAM, the host has plenty.
#if defined(__ARM_ARCH_7EM__)
inline constexpr std::size_t max_particles = 256;
#else
inline constexpr std::size_t max_particles = 4096;
#endif

struct Particles {
    std::array<float, max_particles> x{};
    std::array<float, max_particles> y{};
    std::array<float, max_particles> vx{};
    std::array<float, max_particles> vy{};
    std::array<std::uint8_t, max_particles> alive{};
    std::size_t count{};
};

void make_particles(Particles& particles, std::size_t count);

// Same integration as the classic version, over dense float arrays.
void update(Particles& particles, float dt);

[[nodiscard]] std::size_t count_alive(const Particles& particles);

// WHAT CHANGED VS THE CLASSIC VERSION:
// - cache lines carry only data the loop reads (x,vx then y,vy)
// - the compiler auto-vectorizes the dense float arrays
// - capacity is a compile-time budget: no realloc, no heap, no surprises

} // namespace demo03::modern
