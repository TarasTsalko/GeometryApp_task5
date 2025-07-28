#pragma once

#include <cmath>
#include <stdexcept>
#include <tuple>
#include <vector>

#ifndef GEOMETRY_EPSILON
#define GEOMETRY_EPSILON 1e-10
#endif

namespace geometry {

struct Point2D;
}

namespace geometry::math_utils {

inline constexpr double EPSILON = GEOMETRY_EPSILON;

double CrossProduct(const Point2D &p1, const Point2D &middle, const Point2D &p2);

bool IsPointInPolygon(const Point2D &point, const std::vector<Point2D> &poly);

bool AllPointsInsideOrOnHull(const std::vector<Point2D> &points, const std::vector<Point2D> &poly);

bool AllHullPointsAreExtreme(const std::vector<Point2D> &points, const std::vector<Point2D> &poly);

bool IsCounterClockwise(const std::vector<Point2D> &hull);

void SortPointsCounterClockwise(Point2D &a, Point2D &b, Point2D &c);

}  // namespace geometry::math_utils

namespace geometry {
struct Point2D {
    double x, y;

    constexpr Point2D() : x(0), y(0) {}
    constexpr Point2D(double x, double y) : x(x), y(y) {}

    // Comparison
    [[nodiscard]] constexpr bool operator<(const Point2D &other) const noexcept {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }
    [[nodiscard]] constexpr bool operator==(const Point2D &other) const noexcept {
        // точки считаются совпадающими с заданной точностью (если квадрат расстояние между ними меньше EPSILON)
        // используем квадрат расстояния для оптимизации
        using namespace math_utils;
        return this->SquaredDistanceTo(other) < EPSILON;
    }

    // Binary math operators
    [[nodiscard]] Point2D operator+(const Point2D &other) const noexcept { return {x + other.x, y + other.y}; }
    // операторы должны быть const и noexcept, так как методы в которых они вызываются const
    // и noexcept
    [[nodiscard]] Point2D operator-(const Point2D &other) const noexcept { return {x - other.x, y - other.y}; }
    [[nodiscard]] Point2D operator*(double value) const noexcept { return {x * value, y * value}; }
    [[nodiscard]] Point2D operator/(double value) const {
        if (value == 0.0)
            throw std::runtime_error("Point2D_operator/:Поппытка деления на ноль");
        return {x / value, y / value};
    }

    // Binary geometry operations
    [[nodiscard]] double Dot(const Point2D &other) const noexcept { return x * other.x + y * other.y; }
    [[nodiscard]] double Cross(const Point2D &other) const noexcept { return x * other.y - y * other.x; }
    [[nodiscard]] double Length() const noexcept { return std::sqrt(x * x + y * y); }
    [[nodiscard]] double DistanceTo(const Point2D &other) const noexcept { return (*this - other).Length(); }

    [[nodiscard]] Point2D Normalize() const noexcept {
        const double len = Length();
        return len > 0 ? Point2D{x / len, y / len} : Point2D{0, 0};
    }

    [[nodiscard]] double SquaredDistanceTo(const Point2D &other) const noexcept {
        const Point2D diff = *this - other;
        return diff.Dot(diff);
    }
};

}  // namespace geometry