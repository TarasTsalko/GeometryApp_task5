#include <gtest/gtest.h>
#include <iostream>

#include "geometry.hpp"
#include "intersections.hpp"

namespace geometry::intersections {

TEST(LineLineIntersection, LineLineIntersection) {
    const Line l0({1, 1}, {10, 10});
    const Line l1({10, 1}, {1, 10});
    const auto res = GetIntersectPoints(l0, l1);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value().size(), 1ul);
    const Point2D point(5.5, 5.5);
    ASSERT_EQ(res.value().at(0ul), point);
}

TEST(LineLineParallel, LineLineParallel) {
    const Line l0({1, 1}, {10, 1});
    const Line l1({1, 2}, {10, 2});
    const auto res = GetIntersectPoints(l0, l1);
    ASSERT_EQ(res, std::nullopt);
}

TEST(LineLineCommonPoint, LineLineCommonPoint) {
    const Line l0({0, 0}, {5, 5});
    const Line l1({5, 5}, {10, 10});
    const auto res = GetIntersectPoints(l0, l1);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value().size(), 1ul);
    const Point2D point(5, 5);
    ASSERT_EQ(res.value().at(0ul), point);
}

TEST(CoincidingLineLine, CoincidingLineLine) {
    const Line l0({-3.0, 4.0}, {2.0, -1.0});
    const Line l1({-3.0, 4.0}, {2.0, -1.0});
    const auto res = GetIntersectPoints(l0, l1);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value().size(), 2ul);  // если отрезки совподают, то возвращается 2 точки начала и конца отрезка
    const Point2D point0(-3.0, 4.0), point1{2.0, -1.0};
    ASSERT_EQ(res.value().at(0ul), point0);
    ASSERT_EQ(res.value().at(1ul), point1);
}

TEST(LineLineOverlaping, LineLineOverlaping) {
    const Line l0({1.0, 1.0}, {6.0, 6.0});
    const Line l1({3.0, 3.0}, {8.0, 8.0});
    const auto res = GetIntersectPoints(l0, l1);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value().size(), 2ul);  // если отрезки совподают, то возвращается 2 точки начала и конца отрезка
    const Point2D point0(3.0, 3.0), point1{6.0, 6.0};
    ASSERT_EQ(res.value().at(0ul), point0);
    ASSERT_EQ(res.value().at(1ul), point1);
}

}  // namespace geometry::intersections