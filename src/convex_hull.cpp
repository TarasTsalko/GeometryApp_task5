#include "convex_hull.hpp"
#include "geometry.hpp"
#include <algorithm>
#include <expected>
#include <vector>

namespace geometry::convex_hull {

double CrossProduct(Point2D p1, Point2D middle, Point2D p2) {
    auto new_p1 = p1 - middle;
    auto new_p2 = p2 - middle;
    return new_p1.Cross(new_p2);
}

// Шаблонная функция для построения части оболочки
template <typename Iterator>
GeometryResult<bool> buildHullPartImpl(Iterator begin, Iterator end, StackForGrahamScan &hull, size_t minSize) {
    for (auto it = begin; it != end; ++it) {
        const auto &point = *it;
        while (hull.Size() >= minSize) {
            const auto res = hull.IsLeftTurnOrCollinear(point);
            if (res.has_value()) {
                if (res.value())
                    break;
            } else {
                return res;
            }
        }
        hull.Push(point);
    }
    return true;
}

// Обёртка для вызова с разными типами итераторов
GeometryResult<bool> buildHullPart(const std::vector<Point2D> &points, StackForGrahamScan &hull, bool reverse = false,
                                   size_t minSize = 2) {
    if (!reverse) {
        return buildHullPartImpl(points.begin(), points.end(), hull, minSize);
    } else {
        return buildHullPartImpl(points.rbegin(), points.rend(), hull, minSize);
    }
}

template <typename T>
    requires requires(T a, T b) {
        { a.y };
        { b.y };
        { a.x };
        { b.x };
    }
auto compare_points = [](const T &a, const T &b) { return std::tie(a.y, a.x) < std::tie(b.y, b.x); };

GeometryResult<std::vector<Point2D>> GrahamScan(std::vector<Point2D> &points) {
    if (points.size() < 3)
        return std::unexpected{GeometryError::InvalidInput};

    // Функция сравнения для сортировки точек:
    // Сначала сортируем по y-координате, затем по x-координате
    // Находим самую нижнюю левую точку и сортируем остальные
    auto compare = [](const Point2D &a, const Point2D &b) {
        if (a.y == b.y)
            return a.x < b.x;
        return a.y < b.y;
    };
    std::ranges::sort(points, compare);

    // Создаем стек для хранения точек выпуклой оболочки
    StackForGrahamScan hull;

    // Строим верхнюю оболочку
    auto res = buildHullPart(points, hull, false, 2);
    if (!res)
        return std::unexpected(res.error());

    // Сохраняем размер верхней оболочки
    const size_t lowerSize = hull.Size();

    // Строим нижнюю оболочку
    res = buildHullPart(points, hull, true, lowerSize);
    if (!res)
        return std::unexpected(res.error());

    // Удаляем дубль начальной точки
    hull.RemoveDuplicateStartPoint(points[0]);

    std::vector<Point2D> result;
    result.reserve(hull.Size());
    while (!hull.Empty()) {
        result.push_back(hull.Top());
        hull.Pop();
    }

    return result;
}

}  // namespace geometry::convex_hull