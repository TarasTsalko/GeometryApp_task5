#include <gtest/gtest.h>

#include "geometry.hpp"

namespace geometry {

TEST(CircleMethods, CircleMethods) {
    const Point2D center(5.0, 7.0);
    Circle circle(center, 15.0);

    ASSERT_EQ(circle.Center(), center);
    ASSERT_DOUBLE_EQ(circle.Radius(), 15.0);

    ASSERT_DOUBLE_EQ(circle.Height(), 22.0);  // centre.y + radius
    const BoundingBox box{-10.0, -8.0, 20.0, 22.0};
    ASSERT_EQ(box, circle.BoundBox());
}

TEST(CircleMethodsNotEnoughPoints, CircleMethodsNotEnoughPoints) {
    const Point2D center(5.0, 7.0);

    try {
        Circle circle(center, 15.0);
        circle.Vertices(2);
    } catch (const std::runtime_error &ex) {
        EXPECT_EQ(std::string_view("At least 3 points are required to define a circle, N = 2"), ex.what());
    }
}

TEST(LineMethods, LineMethods) {
    const Point2D start(2.0, 5.0);
    const Point2D end(-3.0, 1.0);
    const Line line(start, end);
    ASSERT_EQ(Point2D(-5.0, -4.0), line.Direction());
    ASSERT_DOUBLE_EQ(line.Length(), 6.4031242374328485);
    ASSERT_DOUBLE_EQ(line.Height(), 5.0);
    const BoundingBox box{-3.0, 1.0, 2.0, 5.0};
    ASSERT_EQ(box, line.BoundBox());
}

TEST(TriangleMethods, TriangleMethods) {
    const Triangle tri(Point2D{0.0, 0.0}, Point2D{4.0, 0.0}, Point2D{0.0, 3.0});
    ASSERT_EQ(tri.Center(), Point2D(1.3333333333333333, 1.0));
    ASSERT_EQ(tri.Height(), 3.0);
    ASSERT_EQ(tri.Area(), 6.0);
    const BoundingBox box{0.0, 0.0, 4.0, 3.0};
    ASSERT_EQ(tri.BoundBox(), box);
}

TEST(RectangleMethods, RectangleMethods) {
    const Rectangle rect(Point2D{2, 1}, 3, 6);
    ASSERT_DOUBLE_EQ(rect.GetWidth(), 3.0);
    ASSERT_DOUBLE_EQ(rect.GetHeight(), 6.0);
    ASSERT_DOUBLE_EQ(rect.Height(), 7.0);
    ASSERT_EQ(rect.Center(), Point2D(3.5, 4));
    ASSERT_DOUBLE_EQ(rect.Area(), 18.0);
    const BoundingBox box{2.0, 1.0, 5.0, 7.0};
    ASSERT_EQ(rect.BoundBox(), box);
}

TEST(RectangleIncorrectParameters, RectangleIncorrectParameters) {

    try {
        const Rectangle rect(Point2D{2, 1}, -3, -6);
    } catch (const std::runtime_error &ex) {
        EXPECT_EQ(std::string_view("Incorrect parameters for Rectangle width -3, height -6"), ex.what());
    }
}

TEST(CircleIncorrectParameters, CircleIncorrectParameters) {
    try {
        const Circle rect(Point2D{2, 1}, -3);
    } catch (const std::runtime_error &ex) {
        EXPECT_EQ(std::string_view("Incorrect parameters for Circle radius = -3"), ex.what());
    }
}

TEST(RegularPolygonIncorrectParameters, RegularPolygonIncorrectParameters) {
    try {
        const RegularPolygon poly(Point2D{3.5, 7.0}, 0.0, 0);
    } catch (const std::runtime_error &ex) {
        EXPECT_EQ(std::string_view("Incorrect parameters for RegularPolygon radius = 0 and sides = 0"), ex.what());
    }
}

TEST(RegularPolygonMethods, RegularPolygonMethods) {
    const Point2D center = {3.5, 7.0};
    const RegularPolygon poly(center, 100.0, 3);
    ASSERT_DOUBLE_EQ(poly.Height(), 107.0);
    ASSERT_EQ(poly.Center(), center);
    ASSERT_EQ(poly.Sides(), 3);
    ASSERT_EQ(poly.Radius(), 100.0);
    ASSERT_EQ(poly.Vertices().size(), 3);
    ASSERT_EQ(poly.Lines().x.size(), 4);  // первая точка добовляется дважды, чтобы замкнуть
    ASSERT_EQ(poly.Lines().y.size(), 4);  // первая точка добовляется дважды, чтобы замкнуть
    const BoundingBox box{-46.500000000000043, -79.60254037844383, 103.5, 93.602540378443877};
    ASSERT_EQ(box, poly.BoundBox());
}

}  // namespace geometry
// namespace geometry