#include "math_utils.hpp"

#include "geometry.hpp"

namespace geometry::math_utils {

double CrossProduct(const Point2D &p1, const Point2D &middle, const Point2D &p2) {
    const auto new_p1 = p1 - middle;
    const auto new_p2 = p2 - middle;
    return new_p1.Cross(new_p2);
}

}  // namespace geometry::math_utils