#include <gtest/gtest.h>
#include <iostream>

#include "geometry.hpp"
#include "queries.hpp"

namespace geometry::queries {

TEST(DistanceFromPointToCircle, DistanceFromPointToCircle) {
    const Circle circle({3.0, 3.0}, 2.0);
    const Point2D point(10.0, 10.0);
    const double dist = DistanceToPoint(circle, point);
    ASSERT_DOUBLE_EQ(dist, 7.8994949366116654);
}

TEST(DistanceFromPointToRect, DistanceFromPointToRect) {
    const Rectangle rect{{1.0, 1.0}, 4.0, 4.0};
    const Point2D point(10.0, 10.0);
    const double dist = DistanceToPoint(rect, point);
    ASSERT_DOUBLE_EQ(dist, 7.0710678118654755);
}

TEST(DistanceFromPointToLine, DistanceFromPointToLine) {
    const Line line{{7.0, 3.0}, {9.0, 3.0}};
    const Point2D point(10.0, 10.0);
    const double dist = DistanceToPoint(line, point);
    ASSERT_DOUBLE_EQ(dist, 7.0710678118654755);
}

TEST(DistanceFromPointToTriangle, DistanceFromPointToTriangle) {
    const Triangle tri({2.0, 2.0}, {4.0, 4.0}, {3.0, 6.0});
    const Point2D point(10.0, 10.0);
    const double dist = DistanceToPoint(tri, point);
    ASSERT_DOUBLE_EQ(dist, 8.0622577482985491);
}

TEST(DistanceFromPointToRegularPolygon, DistanceFromPointToRegularPolygon) {
    const RegularPolygon poly{{6.0, 4.0}, 1.5, 6};
    const Point2D point(10.0, 10.0);
    const double dist = DistanceToPoint(poly, point);
    ASSERT_DOUBLE_EQ(dist, 5.7150277979971458);
}

TEST(GetHeightCircle, GetHeightCircle) {
    const Circle circle({3.0, 3.0}, 2.0);
    const double dist = GetHeight(circle);
    ASSERT_DOUBLE_EQ(dist, 5.0);
}

TEST(GetHeightRect, GetHeightRect) {
    const Rectangle rect{{1.0, 1.0}, 4.0, 4.0};
    const double dist = GetHeight(rect);
    ASSERT_DOUBLE_EQ(dist, 5.0);
}

TEST(GetHeightLine, GetHeightLine) {
    const Line line{{7.0, 3.0}, {9.0, 3.0}};
    const double dist = GetHeight(line);
    ASSERT_DOUBLE_EQ(dist, 3.0);
}

TEST(GetHeightTriangle, GetHeightTriangle) {
    const Triangle tri({2.0, 2.0}, {4.0, 4.0}, {3.0, 6.0});
    const double dist = GetHeight(tri);
    ASSERT_DOUBLE_EQ(dist, 6.0);
}

TEST(GetHeightRegularPolygon, GetHeightRegularPolygon) {
    const RegularPolygon poly{{6.0, 4.0}, 1.5, 6};
    const double dist = GetHeight(poly);
    ASSERT_DOUBLE_EQ(dist, 5.5);
}

TEST(GetBoundBoxCircle, GetBoundBoxCircle) {
    const Circle circle({3.0, 3.0}, 2.0);
    const auto bb = GetBoundBox(circle);
    const BoundingBox etalonBB(1.0, 1.0, 5.0, 5.0);
    ASSERT_EQ(bb, etalonBB);
}

TEST(GetBoundBoxRect, GetBoundBoxRect) {
    const Circle circle({3.0, 3.0}, 2.0);
    const auto bb = GetBoundBox(circle);
    const BoundingBox etalonBB(1.0, 1.0, 5.0, 5.0);
    ASSERT_EQ(bb, etalonBB);
}

TEST(GetBoundBoxLine, GetBoundBoxLine) {
    const Line line{{7.0, 3.0}, {9.0, 3.0}};
    const auto bb = GetBoundBox(line);
    const BoundingBox etalonBB(7.0, 3.0, 9.0, 3.0);
    ASSERT_EQ(bb, etalonBB);
}

TEST(GetBoundBoxTriangle, GetBoundBoxTriangle) {
    const Triangle tri({2.0, 2.0}, {4.0, 4.0}, {3.0, 6.0});
    const auto bb = GetBoundBox(tri);
    const BoundingBox etalonBB(2.0, 2.0, 4.0, 6.0);
    ASSERT_EQ(bb, etalonBB);
}

TEST(GetBoundBoxRegularPolygon, GetBoundBoxRegularPolygon) {
    const RegularPolygon poly{{6.0, 4.0}, 1.5, 6};
    const auto bb = GetBoundBox(poly);

    const BoundingBox etalonBB(4.5, 2.700961894323342, 7.5, 5.299038105676658);
    ASSERT_EQ(bb, etalonBB);
    ASSERT_DOUBLE_EQ(bb.max_x, etalonBB.max_x);
    ASSERT_DOUBLE_EQ(bb.min_x, etalonBB.min_x);
    ASSERT_DOUBLE_EQ(bb.min_y, etalonBB.min_y);
    ASSERT_DOUBLE_EQ(bb.max_y, etalonBB.max_y);
}

}  // namespace geometry::queries