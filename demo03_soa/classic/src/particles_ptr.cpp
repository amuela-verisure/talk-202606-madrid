#include "demo03/classic/particles_ptr.hpp"

namespace demo03::classic {

PtrParticles make_particles_ptr(std::size_t count) {
    PtrParticles particles;
    particles.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        auto fi = static_cast<float>(i);
        particles.push_back(std::make_unique<PointParticle>(
            fi * 0.5f, fi * -0.25f, 1.0f + fi * 0.001f, -0.5f + fi * 0.002f,
            (i % 4u) != 3u));
    }
    return particles;
}

void update(PtrParticles& particles, float dt) {
    for (auto& p : particles) {
        p->update(dt); // indirect call through the vtable, per particle
    }
}

std::size_t count_alive(const PtrParticles& particles) {
    std::size_t n = 0;
    for (const auto& p : particles) {
        if (p->is_alive()) {
            ++n;
        }
    }
    return n;
}

} // namespace demo03::classic
