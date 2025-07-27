#include "convex_hull.hpp"
#include "geometry.hpp"
#include "math_utils.hpp"
#include <algorithm>
#include <cstddef>
#include <expected>
#include <span>
#include <vector>

#include <stack>

namespace geometry::convex_hull {

class StackForGrahamScan {

public:
    void Pop() { hull.pop(); }

    [[nodiscard]] const Point2D &Top() const { return hull.top(); }

    void Push(const Point2D &point) { hull.push(point); }

    [[nodiscard]] size_t Size() const noexcept { return hull.size(); };

    [[nodiscard]] bool Empty() const noexcept { return hull.empty(); }

    // Метод для проверки и обработки точек в стеке
    GeometryResult<bool> IsLeftTurnOrCollinear(const Point2D &point) {
        if (hull.size() < 2)
            return std::unexpected<GeometryError>(GeometryError::InsufficientPoints);

        using namespace geometry::math_utils;
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
GeometryResult<bool> buildHullPart(const std::span<Point2D> points, StackForGrahamScan &hull, bool reverse = false,
                                   size_t minSize = 2) {
    if (!reverse) {
        return buildHullPartImpl(points.begin(), points.end(), hull, minSize);
    } else {
        return buildHullPartImpl(points.rbegin(), points.rend(), hull, minSize);
    }
}

// может span, уточнить у ревьювира
// UPD: уточнил у ревьювера, заменил на const std::span<Point2D> points
GeometryResult<std::vector<Point2D>> GrahamScan(std::span<Point2D> points) {
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