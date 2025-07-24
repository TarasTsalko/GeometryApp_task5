#pragma once

namespace geometry {

struct Point2D;

}

namespace geometry::math_utils {

double CrossProduct(const Point2D &p1, const Point2D &middle, const Point2D &p2);

}  // namespace geometry::math_utils