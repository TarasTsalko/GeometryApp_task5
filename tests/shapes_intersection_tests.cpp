#include <gtest/gtest.h>
#include <iostream>

#include "geometry.hpp"
#include "intersections.hpp"

namespace geometry::intersections {

TEST(LineLineIntersection, LineLineIntersection) {
    Line l0({1, 1}, {10, 10});
    Line l1({10, 1}, {1, 10});
    const auto res = GetIntersectPoints(l0, l1);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value().size(), 1ul);
    const Point2D point(5.5, 5.5);
    ASSERT_EQ(res.value().at(0ul), point);
}

TEST(LineLineParallel, LineLineParallel) {
    Line l0({1, 1}, {10, 1});
    Line l1({1, 2}, {10, 2});
    const auto res = GetIntersectPoints(l0, l1);
    ASSERT_EQ(res, std::nullopt);
}

TEST(LineLineCommonPoint, LineLineCommonPoint) {
    Line l0({0, 0}, {5, 5});
    Line l1({5, 5}, {10, 10});
    const auto res = GetIntersectPoints(l0, l1);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value().size(), 1ul);
    const Point2D point(5, 5);
    ASSERT_EQ(res.value().at(0ul), point);
}

}  // namespace geometry::intersections