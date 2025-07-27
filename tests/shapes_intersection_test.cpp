#include <gtest/gtest.h>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "geometry.hpp"
#include "intersections.hpp"
#include "queries.hpp"
#include "shape_utils.hpp"

namespace geometry::intersections {

TEST(LineLineIntersection, LineLineIntersection) {
    const Line l0({1, 1}, {10, 10});
    const Line l1({10, 1}, {1, 10});
    const auto res = GetIntersectPoints(l0, l1);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value().points.size(), 1ul);
    const Point2D point(5.5, 5.5);
    const auto intersectionType = res.value().type;
    const auto &points = res.value().points;
    ASSERT_EQ(intersectionType, IntersectionType::Point);
    ASSERT_EQ(points.at(0ul), point);
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
    const auto intersectionType = res.value().type;
    const auto &points = res.value().points;
    ASSERT_EQ(intersectionType, IntersectionType::Point);
    ASSERT_EQ(points.size(), 1ul);
    const Point2D point(5, 5);
    ASSERT_EQ(points.at(0ul), point);
}

TEST(CoincidingLineLine, CoincidingLineLine) {
    const Line l0({-3.0, 4.0}, {2.0, -1.0});
    const Line l1({-3.0, 4.0}, {2.0, -1.0});
    const auto res = GetIntersectPoints(l0, l1);
    ASSERT_TRUE(res.has_value());
    const auto &points = res.value().points;
    const auto intersectionType = res.value().type;
    ASSERT_EQ(intersectionType, IntersectionType::Coincident);
    ASSERT_EQ(points.size(), 2ul);  // если отрезки совподают, то возвращается 2 точки начала и конца отрезка
    const Point2D point0(-3.0, 4.0), point1{2.0, -1.0};
    ASSERT_EQ(points.at(0ul), point0);
    ASSERT_EQ(points.at(1ul), point1);
}

TEST(LineLineOverlaping, LineLineOverlaping) {
    const Line l0({1.0, 1.0}, {6.0, 6.0});
    const Line l1({3.0, 3.0}, {8.0, 8.0});
    const auto res = GetIntersectPoints(l0, l1);
    ASSERT_TRUE(res.has_value());
    const auto intersectionType = res.value().type;
    ASSERT_EQ(intersectionType, IntersectionType::Segment);
    const auto &points = res.value().points;
    ASSERT_EQ(points.size(), 2ul);  // если отрезки совподают, то возвращается 2 точки начала и конца отрезка
    const Point2D point0(3.0, 3.0), point1{6.0, 6.0};
    ASSERT_EQ(points.at(0ul), point0);
    ASSERT_EQ(points.at(1ul), point1);
}

TEST(CircleCircleIntersection, CircleCircleIntersection) {
    const Circle circle0(Point2D(0.0, 0.0), 5.0);
    const Circle circle1(Point2D(4.0, 0.0), 5.0);
    const auto res = GetIntersectPoints(circle0, circle1);
    const Point2D point0(2.0, 4.5825756949558398), point1{2.0, -4.5825756949558398};
    ASSERT_TRUE(res.has_value());
    const auto intersectionType = res.value().type;
    ASSERT_EQ(intersectionType, IntersectionType::Segment);
    const auto &points = res.value().points;
    ASSERT_EQ(points.size(), 2ul);
    ASSERT_EQ(points.at(0ul), point0);
    ASSERT_EQ(points.at(1ul), point1);
}

TEST(CircleCircleTangency, CircleCircleTangency) {
    const Circle circle0(Point2D(-3.0, 0.0), 3.0);
    const Circle circle1(Point2D(3.0, 0.0), 3.0);
    const auto res = GetIntersectPoints(circle0, circle1);
    const Point2D point(0.0, 0.0);
    ASSERT_TRUE(res.has_value());
    const auto intersectionType = res.value().type;
    ASSERT_EQ(intersectionType, IntersectionType::Point);
    const auto &points = res.value().points;
    ASSERT_EQ(points.size(), 1ul);
    ASSERT_EQ(points.at(0ul), point);
}

TEST(NestedCircleCircle, NestedCircleCircle) {
    const Circle circle0(Point2D(0.0, 0.0), 10.0);
    const Circle circle1(Point2D(0.0, 0.0), 5.0);
    // уточнил у ревьювира, данный случай не считается пересечением
    const auto res = GetIntersectPoints(circle0, circle1);
    ASSERT_FALSE(res.has_value());
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
    const auto intersectionType = res.value().type;
    ASSERT_EQ(intersectionType, IntersectionType::Coincident);
    const auto &points = res.value().points;
    ASSERT_EQ(points.size(), 1ul);
    ASSERT_EQ(points.at(0ul), center);
}

TEST(CircleLineIntersection, CircleLineIntersection) {
    const Circle circle({0.0, 0.0}, 5.0);
    const Line line({-10.0, 4.0}, {10.0, 4.0});
    const auto res = GetIntersectPoints(circle, line);
    ASSERT_TRUE(res.has_value());
    const auto intersectionType = res.value().type;
    ASSERT_EQ(intersectionType, IntersectionType::Segment);
    const auto &points = res.value().points;
    ASSERT_EQ(points.size(), 2ul);
    ASSERT_EQ(points.at(0ul), Point2D(-3.0, 4.0));
    ASSERT_EQ(points.at(1ul), Point2D(3.0, 4.0));
}

TEST(CircleLineTangency, CircleLineTangency) {
    const Circle circle({0.0, 0.0}, 5.0);
    const Line line({0.0, 5.0}, {5.0, 5.0});
    const auto res = GetIntersectPoints(circle, line);
    ASSERT_TRUE(res.has_value());
    const auto intersectionType = res.value().type;
    ASSERT_EQ(intersectionType, IntersectionType::Point);
    const auto &points = res.value().points;
    ASSERT_EQ(points.size(), 1ul);
    ASSERT_EQ(points.at(0ul), Point2D(0.0, 5.0));
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
    ASSERT_FALSE(res.has_value());  // отрезок внутри (пересечения нет)
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

TEST(FindAllCollisionsTest, FindAllCollisionsTest) {
    const Circle circle({3.0, 3.0}, 2.0);
    const RegularPolygon poly{{6.0, 4.0}, 1.5, 6};
    const Rectangle rect{{1.0, 1.0}, 4.0, 4.0};
    const Rectangle rect2{{10.0, 10.0}, 2.0, 2.0};
    const Triangle tri({2.0, 2.0}, {4.0, 4.0}, {3.0, 6.0});
    const Line line{{7.0, 3.0}, {9.0, 3.0}};
    const std::vector<Shape> shapes = {circle, poly, rect, rect2, tri, line};

    auto collisions = geometry::utils::FindAllCollisions(shapes);
    ASSERT_EQ(collisions.size(), 6ul);
    for (const auto &[shape0, shape1] : collisions) {
        ASSERT_TRUE(geometry::queries::BoundingBoxesOverlap(shape0, shape1));
    }
}

}  // namespace geometry::intersections