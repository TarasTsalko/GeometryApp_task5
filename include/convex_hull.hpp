#pragma once
#include "geometry.hpp"
#include <algorithm>
#include <cstddef>
#include <expected>
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

    // Метод для проверки и обработки точек в стеке
    GeometryResult<bool> IsLeftTurnOrCollinear(const Point2D &point) {
        if (hull.size() < 2)
            return std::unexpected<GeometryError>(GeometryError::InsufficientPoints);

        Point2D top = hull.top();
        hull.pop();
        Point2D nextToTop = hull.top();
        const double cross = CrossProduct(point, top, nextToTop);
        if (cross >= 0.0) {  // >= 0 для обработки коллинеарных точек
            hull.push(top);
            return true;
        }
        return false;
    }

    void RemoveDuplicateStartPoint(const Point2D &point) noexcept {
        // Удаляем дублирующуюся начальную точку
        if (hull.size() > 1 && hull.top() == point)
            hull.pop();
    }

private:
    std::stack<Point2D> hull;
};

GeometryResult<std::vector<Point2D>> GrahamScan(std::vector<Point2D> &points);

}  // namespace geometry::convex_hull