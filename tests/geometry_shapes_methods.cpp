#include <gtest/gtest.h>

#include "geometry.hpp"

namespace geometry {

TEST(CircleMethods, CircleMethods) {
    const Point2D center(5.0, 7.0);
    Circle circle(center, 15.0);

    ASSERT_EQ(circle.Center(), center);
    ASSERT_DOUBLE_EQ(circle.radius, 15.0);

    ASSERT_DOUBLE_EQ(circle.Height(), 22.0);  // centre.y + radius
    const BoundingBox box{-10.0, -8.0, 20.0, 22.0};
    ASSERT_EQ(box, circle.BoundBox());
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
    ASSERT_DOUBLE_EQ(rect.width, 3.0);
    ASSERT_DOUBLE_EQ(rect.height, 6.0);
    ASSERT_DOUBLE_EQ(rect.Height(), 7.0);
    ASSERT_EQ(rect.Center(), Point2D(3.5, 4));
    ASSERT_DOUBLE_EQ(rect.Area(), 18.0);
    const BoundingBox box{2.0, 1.0, 5.0, 7.0};
    ASSERT_EQ(rect.BoundBox(), box);
}

TEST(RegularPolygonMethods, RegularPolygonMethods) {
    const Point2D center = {3.5, 7.0};
    const RegularPolygon poly(center, 100.0, 3);
    ASSERT_DOUBLE_EQ(poly.Height(), 107.0);
    ASSERT_EQ(poly.Center(), center);
    ASSERT_EQ(poly.sides, 3);
    ASSERT_EQ(poly.radius, 100.0);
    ASSERT_EQ(poly.Vertices().size(), 3);
    ASSERT_EQ(poly.Lines().x.size(), 4);  // первая точка добовляется дважды, чтобы замкнуть
    ASSERT_EQ(poly.Lines().y.size(), 4);  // первая точка добовляется дважды, чтобы замкнуть
    const BoundingBox box{-46.500000000000043, -79.60254037844383, 103.5, 93.602540378443877};
    ASSERT_EQ(box, poly.BoundBox());
}

}  // namespace geometry
// namespace geometry