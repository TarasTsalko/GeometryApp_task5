#include "math_utils.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <print>

namespace geometry::math_utils {

double CrossProduct(const Point2D &p1, const Point2D &middle, const Point2D &p2) {
    const auto new_p1 = p1 - middle;
    const auto new_p2 = p2 - middle;
    return new_p1.Cross(new_p2);
}

bool IsPointInPolygon(const Point2D &point, const std::vector<Point2D> &poly) {
    if (poly.size() < 3)
        return false;  // Многоугольник должен иметь минимум 3 точки

    // Проверяем каждую сторону многоугольника
    for (size_t i = 0; i < poly.size(); ++i) {
        const Point2D &p1 = poly[i];
        const Point2D &p2 = poly[(i + 1) % poly.size()];

        // Вычисляем векторное произведение
        const double cross = CrossProduct(point, p1, p2);

        // Если результат положительный, точка лежит снаружи (так как CCW)
        if (cross > EPSILON)
            return false;
    }
    return true;
}

bool AllPointsInsideOrOnHull(const std::vector<Point2D> &points, const std::vector<Point2D> &poly) {

    if (points.empty() || poly.size() < 3ul)
        return false;

    return std::ranges::all_of(points, [&poly](const Point2D &point) { return IsPointInPolygon(point, poly); });
}

bool IsExtremePoint(const Point2D &point, const std::vector<Point2D> &points) {
    if (points.size() < 3)
        return false;

    const double pi = std::acos(-1.0);
    const int angle_step = 10;  // Увеличиваем шаг для оптимизации

    // Проверяем направления с заданным шагом
    for (int i = 0; i < 360; i += angle_step) {
        double angle = i * pi / 180.0;
        Point2D direction = {std::cos(angle), std::sin(angle)};

        double max_dot = direction.Dot(point);
        bool is_extreme = true;

        // Оптимизированный поиск максимума
        for (const auto &p : points) {
            if (direction.Dot(p) > max_dot) {
                is_extreme = false;
                break;
            }
        }

        if (is_extreme)
            return true;
    }

    // Оптимизированная проверка направлений к другим точкам
    for (const auto &dir_point : points) {
        if (point == dir_point)
            continue;

        const Point2D direction = (dir_point - point).Normalize();
        double max_dot = direction.Dot(point);

        bool is_extreme = true;
        for (const auto &p : points) {
            if (direction.Dot(p) > max_dot) {
                is_extreme = false;
                break;
            }
        }

        if (is_extreme)
            return true;
    }

    return false;
}

bool AllHullPointsAreExtreme(const std::vector<Point2D> &points, const std::vector<Point2D> &poly) {
    return std::ranges::all_of(poly, [&](const auto &point) { return IsExtremePoint(point, points); });
}

bool IsCounterClockwise(const std::vector<Point2D> &poly) {
    // Проверка на минимальное количество точек
    if (poly.size() < 3)
        return false;

    // Вычисление ориентированной площади многоугольника
    double signedArea = 0.0;
    const size_t n = poly.size();

    // Проход по всем рёбрам многоугольника
    for (size_t i = 0; i < n; ++i) {
        const Point2D &p1 = poly[i];
        const Point2D &p2 = poly[(i + 1) % n];
        signedArea += p1.Cross(p2);
    }

    // Определение ориентации
    return signedArea > 0;
}

}  // namespace geometry::math_utils