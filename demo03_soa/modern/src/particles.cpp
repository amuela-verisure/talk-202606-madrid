#include "demo03/modern/particles.hpp"

namespace demo03::modern {

void make_particles(Particles& particles, std::size_t count) {
    if (count > max_particles) {
        count = max_particles;
    }
    particles.count = count;
    for (std::size_t i = 0; i < count; ++i) {
        auto fi = static_cast<float>(i);
        particles.x[i] = fi * 0.5f;
        particles.y[i] = fi * -0.25f;
        particles.vx[i] = 1.0f + fi * 0.001f;
        particles.vy[i] = -0.5f + fi * 0.002f;
        particles.alive[i] = (i % 4u) != 3u ? 1u : 0u;
    }
}

void update(Particles& particles, float dt) {
    const std::size_t n = particles.count;
    for (std::size_t i = 0; i < n; ++i) {
        const float mask = particles.alive[i] != 0u ? 1.0f : 0.0f;
        particles.x[i] += particles.vx[i] * dt * mask;
        particles.y[i] += particles.vy[i] * dt * mask;
    }
}

std::size_t count_alive(const Particles& particles) {
    std::size_t n = 0;
    for (std::size_t i = 0; i < particles.count; ++i) {
        if (particles.alive[i] != 0u) {
            ++n;
        }
    }
    return n;
}

} // namespace demo03::modern
