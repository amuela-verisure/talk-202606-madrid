// Micro-benchmark: the storage evolution for a hot update loop.
//   stage 1: std::vector<std::unique_ptr<IParticle>>  (heap objects + vtable)
//   stage 2: std::vector<Particle>                    (AoS — contiguous values)
//   stage 3: Struct-of-Arrays                         (SoA — parallel arrays)
// Build host-release and run: ./build/host-release/bench_soa
#include <chrono>
#include <cstdio>
#include <memory>

#include "demo03/classic/particles.hpp"
#include "demo03/classic/particles_ptr.hpp"
#include "demo03/modern/particles.hpp"

namespace {

constexpr std::size_t particle_count = 4096;
constexpr int iterations = 50000;
constexpr float dt = 0.016f;

volatile float sink; // defeat dead-code elimination

template <typename F>
double time_ms(F&& body) {
    const auto start = std::chrono::steady_clock::now();
    body();
    const auto stop = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

double ns_per_particle(double total_ms) {
    return total_ms * 1e6 / (static_cast<double>(iterations) * particle_count);
}

} // namespace

int main() {
    auto ptr_particles = demo03::classic::make_particles_ptr(particle_count);
    auto aos_particles = demo03::classic::make_particles(particle_count);
    auto soa_particles = std::make_unique<demo03::modern::Particles>();
    demo03::modern::make_particles(*soa_particles, particle_count);

    // Warm-up
    demo03::classic::update(ptr_particles, dt);
    demo03::classic::update(aos_particles, dt);
    demo03::modern::update(*soa_particles, dt);

    const double ptr_ms = time_ms([&] {
        for (int i = 0; i < iterations; ++i) {
            demo03::classic::update(ptr_particles, dt);
        }
    });
    sink = ptr_particles[1]->pos_x();

    const double aos_ms = time_ms([&] {
        for (int i = 0; i < iterations; ++i) {
            demo03::classic::update(aos_particles, dt);
        }
    });
    sink = aos_particles[1].x;

    const double soa_ms = time_ms([&] {
        for (int i = 0; i < iterations; ++i) {
            demo03::modern::update(*soa_particles, dt);
        }
    });
    sink = soa_particles->x[1];

    std::printf("particles: %zu, iterations: %d\n", particle_count, iterations);
    std::printf("%-32s %10s %14s %12s\n", "stage", "total ms", "ns/particle",
                "vs stage 1");
    std::printf("%-32s %10.1f %14.3f %11.2fx\n",
                "1: vector<unique_ptr<IParticle>>", ptr_ms,
                ns_per_particle(ptr_ms), 1.0);
    std::printf("%-32s %10.1f %14.3f %11.2fx\n", "2: vector<Particle> (AoS)",
                aos_ms, ns_per_particle(aos_ms), ptr_ms / aos_ms);
    std::printf("%-32s %10.1f %14.3f %11.2fx\n", "3: Struct-of-Arrays (SoA)",
                soa_ms, ns_per_particle(soa_ms), ptr_ms / soa_ms);
    return 0;
}
