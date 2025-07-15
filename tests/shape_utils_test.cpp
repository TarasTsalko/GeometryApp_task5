#include <gtest/gtest.h>

#include "geometry.hpp"
#include "shape_utils.hpp"

namespace geometry::utils {

TEST(FindHighestShapeTests, FindHighestShapeTests) {
    const Circle circle({3.0, 3.0}, 2.0);
    const RegularPolygon poly{{6.0, 4.0}, 1.5, 6};
    const Rectangle rect{{1.0, 1.0}, 4.0, 4.0};
    const Rectangle rect2{{10.0, 10.0}, 2.0, 2.0};
    const Triangle tri({2.0, 2.0}, {4.0, 4.0}, {3.0, 6.0});
    const Line line{{7.0, 3.0}, {9.0, 3.0}};
    const std::vector<Shape> shapes = {circle, poly, rect, rect2, tri, line};
    const auto res = FindHighestShape(shapes);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value(), 3ul);
}

TEST(FindHighestShapeEmptyTests, FindHighestShapeEmptyTests) {
    const std::vector<Shape> shapes;
    const auto res = FindHighestShape(shapes);
    ASSERT_FALSE(res.has_value());
}

}  // namespace geometry::utils