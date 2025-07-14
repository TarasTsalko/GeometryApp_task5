#include <gtest/gtest.h>
#include <stdexcept>
#include <string_view>

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

TEST(CircleCircleIntersection, CircleCircleIntersection) {
    const Circle circle0(Point2D(0.0, 0.0), 5.0);
    const Circle circle1(Point2D(4.0, 0.0), 5.0);
    const auto res = GetIntersectPoints(circle0, circle1);
    const Point2D point0(2.0, 4.5825756949558398), point1{2.0, -4.5825756949558398};
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value().size(), 2ul);
    ASSERT_EQ(res.value().at(0ul), point0);
    ASSERT_EQ(res.value().at(1ul), point1);
}

TEST(CircleCircleTangency, CircleCircleTangency) {
    const Circle circle0(Point2D(-3.0, 0.0), 3.0);
    const Circle circle1(Point2D(3.0, 0.0), 3.0);
    const auto res = GetIntersectPoints(circle0, circle1);
    const Point2D point(0.0, 0.0);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value().size(), 1ul);
    ASSERT_EQ(res.value().at(0ul), point);
}

TEST(NestedCircleCircle, NestedCircleCircle) {
    const Circle circle0(Point2D(0.0, 0.0), 10.0);
    const Circle circle1(Point2D(0.0, 0.0), 5.0);
    const auto res = GetIntersectPoints(circle0, circle1);
    const Point2D point(0.0, 0.0);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value().size(), 1ul);
    ASSERT_EQ(res.value().at(0ul), point);
}

TEST(NotIntersectedCircleCircle, NotIntersectedCircleCircle) {
    const Circle circle0(Point2D(-5.0, 0.0), 3.0);
    const Circle circle1(Point2D(5.0, 0.0), 3.0);
    const auto res = GetIntersectPoints(circle0, circle1);
    ASSERT_FALSE(res.has_value());
}

TEST(CoincidingCircleCircle, CoincidingCircleCircle) {
    const Point2D center(-5.0, 0.0);
    const Circle circle0(center, 3.0);
    const Circle circle1(center, 3.0);
    const auto res = GetIntersectPoints(circle0, circle1);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value().size(), 1ul);
    ASSERT_EQ(res.value().at(0ul), center);
}

TEST(CircleLineIntersection, CircleLineIntersection) {
    const Circle circle({0.0, 0.0}, 5.0);
    const Line line({-3.0, 4.0}, {3.0, 4.0});
    const auto res = GetIntersectPoints(circle, line);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value().size(), 2ul);
    ASSERT_EQ(res.value().at(0ul), line.start);
    ASSERT_EQ(res.value().at(1ul), line.end);
}

TEST(CircleLineTangency, CircleLineTangency) {
    const Circle circle({0.0, 0.0}, 5.0);
    const Line line({0.0, 5.0}, {5.0, 5.0});
    const auto res = GetIntersectPoints(circle, line);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value().size(), 1ul);
    ASSERT_EQ(res.value().at(0ul), Point2D(0.0, 5.0));
}

TEST(NotIntersectdCircleLine, NotIntersectdCircleLine) {
    const Circle circle({0.0, 0.0}, 3.0);
    const Line line({5.0, 5.0}, {6.0, 6.0});
    const auto res = GetIntersectPoints(circle, line);
    ASSERT_FALSE(res.has_value());
}

TEST(LineInsideCircle, LineInsideCircle) {
    const Circle circle({0.0, 0.0}, 10.0);
    const Line line({1.0, 1.0}, {2.0, 2.0});
    const auto res = GetIntersectPoints(circle, line);
    ASSERT_TRUE(res.has_value());  // отрезок внут
    ASSERT_EQ(res.value().size(), 2ul);
    ASSERT_EQ(res.value().at(0ul), line.start);
    ASSERT_EQ(res.value().at(1ul), line.end);
}

TEST(NotSupportedIntersectionTests, NotSupportedIntersectionTests) {
    const Circle circle({0.0, 0.0}, 10.0);
    const Rectangle rect{{0.0, 0.0}, 5.0, 7.0};
    try {
        const auto res = GetIntersectPoints(circle, rect);

    } catch (const std::logic_error &ex) {
        EXPECT_EQ(std::string_view("Unexpected shapes for intersection operation"), ex.what());
    }
}

}  // namespace geometry::intersections