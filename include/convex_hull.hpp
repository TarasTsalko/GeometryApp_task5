#pragma once
#include "geometry.hpp"
#include <expected>
#include <vector>

namespace geometry::convex_hull {

GeometryResult<std::vector<Point2D>> GrahamScan(std::span<Point2D> points);

}  // namespace geometry::convex_hull