#include "demo03/classic/particles.hpp"

namespace demo03::classic {

Particles make_particles(std::size_t count) {
    Particles particles(count);
    for (std::size_t i = 0; i < count; ++i) {
        auto fi = static_cast<float>(i);
        particles[i].x = fi * 0.5f;
        particles[i].y = fi * -0.25f;
        particles[i].vx = 1.0f + fi * 0.001f;
        particles[i].vy = -0.5f + fi * 0.002f;
        particles[i].id = static_cast<std::uint32_t>(i);
        particles[i].alive = (i % 4u) != 3u; // every 4th particle is dead
    }
    return particles;
}

void update(Particles& particles, float dt) {
    for (auto& p : particles) {
        if (p.alive) {
            p.x += p.vx * dt;
            p.y += p.vy * dt;
        }
    }
}

std::size_t count_alive(const Particles& particles) {
    std::size_t n = 0;
    for (const auto& p : particles) {
        if (p.alive) {
            ++n;
        }
    }
    return n;
}

} // namespace demo03::classic
