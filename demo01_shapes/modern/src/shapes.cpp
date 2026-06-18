#include "demo01/modern/shapes.hpp"

namespace demo01::modern {

Scene make_scene() {
    return Scene{
        Circle{1.0f},          Rectangle{2.0f, 3.0f}, Triangle{4.0f, 5.0f},
        Circle{0.5f},          Rectangle{1.5f, 1.5f}, Triangle{2.5f, 6.0f},
    };
}

float total_area(const Scene& scene) {
    float sum = 0.0f;
    for (const auto& shape : scene) {
        sum += std::visit([](const auto& s) { return s.area(); }, shape);
    }
    return sum;
}

void scale_all(Scene& scene, float factor) {
    for (auto& shape : scene) {
        std::visit([factor](auto& s) { s.scale(factor); }, shape);
    }
}

} // namespace demo01::modern
