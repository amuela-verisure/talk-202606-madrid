#pragma once
// Classic OOP polymorphism: virtual base class, heap-allocated objects,
// owning pointers, runtime dispatch through the vtable.
#include <cstddef>
#include <memory>
#include <vector>

namespace demo01::classic {

class Shape {
public:
    virtual ~Shape() = default;
    [[nodiscard]] virtual float area() const = 0;
    virtual void scale(float factor) = 0;
};

class Circle final : public Shape {
public:
    explicit Circle(float radius) : radius_{radius} {}
    [[nodiscard]] float area() const override;
    void scale(float factor) override { radius_ *= factor; }

private:
    float radius_;
};

class Rectangle final : public Shape {
public:
    Rectangle(float width, float height) : width_{width}, height_{height} {}
    [[nodiscard]] float area() const override;
    void scale(float factor) override {
        width_ *= factor;
        height_ *= factor;
    }

private:
    float width_;
    float height_;
};

class Triangle final : public Shape {
public:
    Triangle(float base, float height) : base_{base}, height_{height} {}
    [[nodiscard]] float area() const override;
    void scale(float factor) override {
        base_ *= factor;
        height_ *= factor;
    }

private:
    float base_;
    float height_;
};

// Every element is a separate heap allocation; the vector itself is another one.
using Scene = std::vector<std::unique_ptr<Shape>>;

[[nodiscard]] Scene make_scene();
[[nodiscard]] float total_area(const Scene& scene);
void scale_all(Scene& scene, float factor);

// PROBLEMS THIS DEMO MEASURES:
// - one heap allocation per shape + vector storage (fragmentation, nondeterminism)
// - vtable indirect call per area()/scale() (no inlining possible across the call)
// - intrusive design: every shape must inherit from Shape to participate

} // namespace demo01::classic
