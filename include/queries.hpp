#pragma once
#include "geometry.hpp"
#include <cassert>
#include <iostream>
#include <optional>
#include <utility>
#include <variant>

namespace geometry::queries {

template <class... Ts>
struct Multilambda : Ts... {
    using Ts::operator()...;
};

/*
 * Класс для поиска расстояния от точки до фигуры
 *
 * Требуется организовать возможность нахождения расстояния для всех возможных фигур типа-суммы Shape
 */
struct PointToShapeDistanceVisitor {
    Point2D point;

    explicit PointToShapeDistanceVisitor(const Point2D &p) : point(p) {}

    /* ваш код здесь */

    [[nodiscard]] double operator()(const Line &line) const { return CalcPointToLineDistance(line); }

    [[nodiscard]] double operator()(const Triangle &triangle) const { return CalcPointToTriangleDistance(triangle); }

    [[nodiscard]] double operator()(const Rectangle &rectangle) const {
        return CalcPointToRectangleDistance(rectangle);
    }

    [[nodiscard]] double operator()(const RegularPolygon &poly) const {
        return CalcPointToRegularPolygonDistance(poly);
    }

    [[nodiscard]] double operator()(const Polygon &poly) const { return CalcPointToPolygonDistance(poly); }

    [[nodiscard]] double operator()(const Circle &circle) const { return CalcPointToCircleDistance(circle); }

    double operator()(const auto &shape1) const {
        throw std::logic_error("Unexpected shape for calc distante to point operation");
        return {};
    }

private:
    [[nodiscard]] double CalcPointToLineDistance(const Line &line) const;

    [[nodiscard]] double CalcPointToSegmentDistance(const Point2D &a, const Point2D &b) const;

    [[nodiscard]] double CalcPointToTriangleDistance(const Triangle &triangle) const;

    [[nodiscard]] double CalcPointToRectangleDistance(const Rectangle &rectangle) const;

    [[nodiscard]] double CalcPointToCircleDistance(const Circle &circle) const;

    [[nodiscard]] double CalcPointToRegularPolygonDistance(const RegularPolygon &poly) const;

    [[nodiscard]] double CalcPointToPolygonDistance(const Polygon &poly) const;
};

/*
 * Функции-помощники
 */
inline double DistanceToPoint(const Shape &shape, const Point2D &point) {

    /* ваш код здесь */
    return std::visit(PointToShapeDistanceVisitor{point}, shape);
}

/*
 * Класс для поиска расстояния между двумя фигурами
 *
 * Требуется организовать возможность нахождения расстояния только для следующих комбинаций фигур:
 *    - Any    & Point
 *    - Line   & Line
 *    - Circle & Circle
 *
 * Для всех остальных требуется вернуть пустое значение
 */
struct ShapeToShapeDistanceVisitor {

    /* ваш код здесь */
    // Это повторение DistanceToPoint, так как Variant не содержит Point2D то для вызова
    // этого случая будет нужна отдельная перегрузка. Уточнить у ревьювера, что так и задуманно?
    [[nodiscard]] double operator()(const Shape &shape, const Point2D &point) const {
        return DistanceToPoint(shape, point);
    }
    [[nodiscard]] double operator()(const Line &line1, const Line &line2) const {
        return CalcLineLineDistance(line1, line2);
    }

    [[nodiscard]] double operator()(const Circle &circle1, const Circle &circle2) const {
        return CalcCircleCircleDistance(circle1, circle2);
    }

    // тут [[nodiscard]] не имеет смысла, как я понимаю
    double operator()(const auto &shape1, const auto &shape2) const {
        throw std::logic_error("Unexpected shapes for calc distante betwenn shapes operation");
        return 0.0;
    }

private:
    [[nodiscard]] double CalcCircleCircleDistance(const Circle &circle1, const Circle &circle2) const;

    [[nodiscard]] double CalcLineLineDistance(const Line &line1, const Line &line2) const;

    [[nodiscard]] bool CheckIntersection(const Line &line1, const Line &line2) const;
};

inline BoundingBox GetBoundBox(const Shape &shape) {

    /* ваш код здесь */
    Multilambda multilambda{[](const Line &line) { return line.BoundBox(); },
                            [](const Triangle &triangle) { return triangle.BoundBox(); },
                            [](const Rectangle &rectangle) { return rectangle.BoundBox(); },
                            [](const Circle &circle) { return circle.BoundBox(); },
                            [](const RegularPolygon &poly) { return poly.BoundBox(); },
                            [](const Polygon &poly) { return poly.BoundBox(); },
                            [](const Shape &shape) {
                                // так как метод BoundBox должен быть реализован у всех объектов,
                                // то данный код должен быть недостежим, то вызываем std::unreachable();
                                std::unreachable();
                                return BoundingBox{};
                            }};
    return std::visit(multilambda, shape);
}

inline double GetHeight(const Shape &shape) {

    /* ваш код здесь */
    Multilambda multilambda{[](const Line &line) { return line.Height(); },
                            [](const Triangle &triangle) { return triangle.Height(); },
                            [](const Rectangle &rectangle) { return rectangle.Height(); },
                            [](const Circle &circle) { return circle.Height(); },
                            [](const RegularPolygon &poly) { return poly.Height(); },
                            [](const Polygon &poly) { return poly.Height(); },
                            [](const Shape &shape) {
                                // так как метод Height должен быть реализован у всех объектов,
                                // то данный код должен быть недостежим, то вызываем std::unreachable();
                                std::unreachable();
                                return BoundingBox{};
                            }};
    return std::visit(multilambda, shape);
}

inline bool BoundingBoxesOverlap(const Shape &shape1, const Shape &shape2) {

    /* ваш код здесь */
    return GetBoundBox(shape1).Overlaps(GetBoundBox(shape2));
}

inline std::optional<double> DistanceBetweenShapes(const Shape &shape1, const Shape &shape2) {

    /* ваш код с ShapeToShapeDistanceVisitor здесь*/
    try {
        return std::visit(ShapeToShapeDistanceVisitor{}, shape1, shape2);
    } catch (const std::logic_error &e) {
        std::cerr << e.what() << std::endl;
        return std::nullopt;
    }
}

inline std::optional<double> DistanceBetweenShapes(const Shape &shape, const Point2D &point) {

    try {
        const double dist = ShapeToShapeDistanceVisitor{}(shape, point);
        return dist;
    } catch (const std::logic_error &e) {
        std::cerr << e.what() << std::endl;
        return std::nullopt;
    }
}

}  // namespace geometry::queries