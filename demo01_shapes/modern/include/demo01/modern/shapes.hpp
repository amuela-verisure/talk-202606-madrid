#pragma once
// Modern closed-set polymorphism: std::variant + std::visit, value semantics,
// contiguous storage on the stack, zero heap allocations, zero vtables.
// See N. Josuttis, "Rethink Polymorphism in C++" (C++ on Sea 2025).
#include <array>
#include <cstddef>
#include <variant>

namespace demo01::modern {

inline constexpr float pi_v = 3.14159265358979323846f;

// Plain value types — no base class, no virtual destructor, no intrusion.
struct Circle {
    float radius;
    [[nodiscard]] constexpr float area() const { return pi_v * radius * radius; }
    constexpr void scale(float factor) { radius *= factor; }
};

struct Rectangle {
    float width;
    float height;
    [[nodiscard]] constexpr float area() const { return width * height; }
    constexpr void scale(float factor) {
        width *= factor;
        height *= factor;
    }
};

struct Triangle {
    float base;
    float height;
    [[nodiscard]] constexpr float area() const { return 0.5f * base * height; }
    constexpr void scale(float factor) {
        base *= factor;
        height *= factor;
    }
};

// The closed set of alternatives IS the polymorphism.
using Shape = std::variant<Circle, Rectangle, Triangle>;

// Contiguous, stack-resident, cache-friendly. sizeof(Shape) is the largest
// alternative + discriminator — no pointer chasing.
inline constexpr std::size_t scene_size = 6;
using Scene = std::array<Shape, scene_size>;

[[nodiscard]] Scene make_scene();
[[nodiscard]] float total_area(const Scene& scene);
void scale_all(Scene& scene, float factor);

// WHAT CHANGED VS THE CLASSIC VERSION:
// - zero heap allocations (std::array of values)
// - std::visit dispatch: jump table the optimizer can inline through
// - non-intrusive: any struct with area()/scale() can join the variant
// - exhaustiveness: adding a Shape alternative without handling it -> compile error

} // namespace demo01::modern
