#include "demo01/classic/shapes.hpp"

namespace demo01::classic {

namespace {
constexpr float pi_v = 3.14159265358979323846f;
}

float Circle::area() const { return pi_v * radius_ * radius_; }

float Rectangle::area() const { return width_ * height_; }

float Triangle::area() const { return 0.5f * base_ * height_; }

Scene make_scene() {
    Scene scene;
    scene.reserve(6);
    scene.push_back(std::make_unique<Circle>(1.0f));
    scene.push_back(std::make_unique<Rectangle>(2.0f, 3.0f));
    scene.push_back(std::make_unique<Triangle>(4.0f, 5.0f));
    scene.push_back(std::make_unique<Circle>(0.5f));
    scene.push_back(std::make_unique<Rectangle>(1.5f, 1.5f));
    scene.push_back(std::make_unique<Triangle>(2.5f, 6.0f));
    return scene;
}

float total_area(const Scene& scene) {
    float sum = 0.0f;
    for (const auto& shape : scene) {
        sum += shape->area(); // indirect call through the vtable
    }
    return sum;
}

void scale_all(Scene& scene, float factor) {
    for (auto& shape : scene) {
        shape->scale(factor); // indirect call through the vtable
    }
}

} // namespace demo01::classic
