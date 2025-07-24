#pragma once

#ifndef GEOMETRY_EPSILON
#define GEOMETRY_EPSILON 1e-10
#endif

namespace geometry {

struct Point2D;
}

namespace geometry::math_utils {

inline constexpr double EPSILON = GEOMETRY_EPSILON;

double CrossProduct(const Point2D &p1, const Point2D &middle, const Point2D &p2);

}  // namespace geometry::math_utils