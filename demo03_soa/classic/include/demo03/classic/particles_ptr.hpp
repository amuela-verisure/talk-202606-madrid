#pragma once
// Stage 1 — where most codebases start: a polymorphic base class and a
// vector of owning pointers. One heap allocation per particle, a vtable
// call per update, and objects scattered all over the heap.
#include <cstddef>
#include <memory>
#include <vector>

namespace demo03::classic {

class IParticle {
public:
    virtual ~IParticle() = default;
    virtual void update(float dt) = 0;
    [[nodiscard]] virtual bool is_alive() const = 0;
    [[nodiscard]] virtual float pos_x() const = 0;
    [[nodiscard]] virtual float pos_y() const = 0;
};

class PointParticle final : public IParticle {
public:
    PointParticle(float x, float y, float vx, float vy, bool alive)
        : x_{x}, y_{y}, vx_{vx}, vy_{vy}, alive_{alive} {}

    void update(float dt) override {
        if (alive_) {
            x_ += vx_ * dt;
            y_ += vy_ * dt;
        }
    }
    [[nodiscard]] bool is_alive() const override { return alive_; }
    [[nodiscard]] float pos_x() const override { return x_; }
    [[nodiscard]] float pos_y() const override { return y_; }

private:
    float x_;
    float y_;
    float vx_;
    float vy_;
    bool alive_;
};

// vtable pointer + heap header per particle; iteration chases pointers.
using PtrParticles = std::vector<std::unique_ptr<IParticle>>;

[[nodiscard]] PtrParticles make_particles_ptr(std::size_t count);
void update(PtrParticles& particles, float dt);
[[nodiscard]] std::size_t count_alive(const PtrParticles& particles);

} // namespace demo03::classic
