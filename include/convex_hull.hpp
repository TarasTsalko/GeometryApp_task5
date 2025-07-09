#pragma once
#include "geometry.hpp"
#include <algorithm>
#include <cstddef>
#include <ranges>
#include <stack>
#include <vector>

namespace geometry::convex_hull {

double CrossProduct(Point2D p1, Point2D middle, Point2D p2);

class StackForGrahamScan {

public:
    void Pop() { hull.pop(); }

    [[nodiscard]] Point2D Top() { return hull.top(); }

    void Push(const Point2D &point) { hull.push(point); }

    [[nodiscard]] size_t Size() const noexcept { return hull.size(); };

    [[nodiscard]] bool Empty() const noexcept { return hull.empty(); }

private:
    std::stack<Point2D> hull;
};

GeometryResult<std::vector<Point2D>> GrahamScan(std::vector<Point2D> &points);

}  // namespace geometry::convex_hull