#include <gtest/gtest.h>

#include "convex_hull.hpp"
#include "shape_utils.hpp"

namespace geometry::convex_hull {

TEST(ConvexHullTest, ConvexHullTest) {
    const Circle circle({3.0, 3.0}, 2.0);
    const RegularPolygon poly{{6.0, 4.0}, 1.5, 6};
    const Rectangle rect{{1.0, 1.0}, 4.0, 4.0};
    const Rectangle rect2{{10.0, 10.0}, 2.0, 2.0};
    const Triangle tri({2.0, 2.0}, {4.0, 4.0}, {3.0, 6.0});
    const Line line{{7.0, 3.0}, {9.0, 3.0}};
    const std::vector<Shape> shapes = {circle, poly, rect, rect2, tri, line};
    auto points = geometry::utils::ColllectAllPoints(shapes);
    auto convex_hull = GrahamScan(points);
    ASSERT_TRUE(convex_hull.has_value());
    const auto resPoints = convex_hull.value();
    ASSERT_FALSE(resPoints.empty());
    ASSERT_EQ(resPoints.size(), 9ul);
    // Вопрос к ревьюверу: как лучше такое протестировать, если сравнивать точки, то может выглядить громостко
}

}  // namespace geometry::convex_hull