#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "demo01/classic/shapes.hpp"
#include "demo01/modern/shapes.hpp"

TEST_CASE("demo01: classic and modern scenes have identical total area") {
    auto classic_scene = demo01::classic::make_scene();
    auto modern_scene = demo01::modern::make_scene();

    CHECK(demo01::classic::total_area(classic_scene) ==
          doctest::Approx(demo01::modern::total_area(modern_scene)));
}

TEST_CASE("demo01: scaling produces identical results in both versions") {
    auto classic_scene = demo01::classic::make_scene();
    auto modern_scene = demo01::modern::make_scene();

    demo01::classic::scale_all(classic_scene, 2.5f);
    demo01::modern::scale_all(modern_scene, 2.5f);

    CHECK(demo01::classic::total_area(classic_scene) ==
          doctest::Approx(demo01::modern::total_area(modern_scene)));
}

TEST_CASE("demo01: modern scene is a stack value — no pointers involved") {
    static_assert(sizeof(demo01::modern::Scene) ==
                  demo01::modern::scene_size * sizeof(demo01::modern::Shape));
    auto scene = demo01::modern::make_scene();
    CHECK(demo01::modern::total_area(scene) > 0.0f);
}
