#include "convex_hull.hpp"
#include "geometry.hpp"
#include <algorithm>
#include <vector>

namespace geometry::convex_hull {

double CrossProduct(Point2D p1, Point2D middle, Point2D p2) {
    auto new_p1 = p1 - middle;
    auto new_p2 = p2 - middle;
    return new_p1.Cross(new_p2);
}

GeometryResult<std::vector<Point2D>> GrahamScan(std::vector<Point2D> &points) {
    if (points.size() < 3)
        return std::unexpected{GeometryError::InsufficientPoints};

    // Находим самую нижнюю левую точку и сортируем остальные
    auto compare = [](const Point2D &a, const Point2D &b) {
        if (a.y == b.y)
            return a.x < b.x;
        return a.y < b.y;
    };
    std::sort(points.begin(), points.end(), compare);

    StackForGrahamScan hull;

    // Строим верхнюю оболочку
    for (const auto &point : points) {
        while (hull.Size() >= 2) {
            Point2D top = hull.Top();
            hull.Pop();
            Point2D nextToTop = hull.Top();

            // Проверяем поворот
            double cross = CrossProduct(point, top, nextToTop);
            if (cross >= 0.0) {  // >= 0 для обработки коллинеарных точек
                hull.Push(top);
                break;
            }
        }
        hull.Push(point);
    }

    // Сохраняем размер верхней оболочки
    const size_t lowerSize = hull.Size();

    // Строим нижнюю оболочку
    for (int i = points.size() - 1; i >= 0; --i) {
        const auto &point = points[i];
        while (hull.Size() > lowerSize) {
            Point2D top = hull.Top();
            hull.Pop();
            Point2D nextToTop = hull.Top();

            double cross = CrossProduct(point, top, nextToTop);
            if (cross >= 0.0) {  // >= 0 для обработки коллинеарных точек
                hull.Push(top);
                break;
            }
        }
        hull.Push(point);
    }

    // Удаляем дублирующуюся начальную точку
    if (hull.Size() > 1 && hull.Top() == points[0])
        hull.Pop();

    std::vector<Point2D> result;
    result.reserve(hull.Size());
    while (!hull.Empty()) {
        result.push_back(hull.Top());
        hull.Pop();
    }

    return result;
}

}  // namespace geometry::convex_hull