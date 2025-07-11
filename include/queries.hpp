#pragma once
#include "geometry.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <optional>
#include <variant>
#include <vector>

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

    double operator()(const Line &line) const {
        const Point2D lineDir = line.end - line.start;
        const Point2D startPointToPointDir = point - line.start;
        // Скалярное произведение
        double dot = lineDir.Dot(startPointToPointDir);
        if (dot <= 0) {
            // Точка ближе всего к P0
            return startPointToPointDir.Length();
        }

        const double dot2 = lineDir.Length();
        if (dot2 <= dot) {
            // Точка ближе всего к P1
            return (point - line.end).Length();
        }

        const double t = dot / dot2;

        const Point2D segmentPoint = line.start + lineDir * t;
        return (point - segmentPoint).Length();
    }

    double operator()(const Triangle &triangle) const {
        const Point2D &A = triangle.a;
        const Point2D &B = triangle.b;
        const Point2D &C = triangle.c;

        auto CalcArea = [](Point2D p1, Point2D p2, Point2D p3) {
            return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
        };

        double d1 = CalcArea(point, A, B);
        double d2 = CalcArea(point, B, C);
        double d3 = CalcArea(point, C, A);

        const bool has_neg = (d1 < -eps) || (d2 < -eps) || (d3 < -eps);
        const bool has_pos = (d1 > eps) || (d2 > eps) || (d3 > eps);

        if (!(has_neg && has_pos)) {
            // Точка внутри треугольника - расстояние равно 0
            return 0.0;
        }

        // 2. Если точка снаружи, вычисляем расстояние до ближайшего ребра
        auto distanceToSegment = [&](Point2D p, Point2D v, Point2D w) {
            double l2 = (v - w).Dot(v - w);
            if (l2 < eps)
                return (p - v).Dot(p - v);

            double t = std::max(0.0, std::min(1.0, (p - v).Dot(w - v) / l2));
            Point2D projection = v + (w - v) * t;
            return sqrt((p - projection).Dot(p - projection));
        };

        const double distAB = distanceToSegment(point, A, B);
        const double distBC = distanceToSegment(point, B, C);
        const double distCA = distanceToSegment(point, C, A);

        return std::min({distAB, distBC, distCA});
    }

    double operator()(const Rectangle &rectangle) const {
        // Угловые точки прямоугольника
        const Point2D &lb = rectangle.bottom_left;
        const Point2D rb = {lb.x + rectangle.width, lb.y};
        const Point2D rt = {lb.x + rectangle.width, lb.y + rectangle.height};
        const Point2D lt = {lb.x, lb.y + rectangle.height};

        // Проверка на нахождение внутри прямоугольника
        if (point.x >= lb.x - eps && point.x <= rb.x + eps && point.y >= lb.y - eps && point.y <= lt.y + eps) {
            return 0.0;
        }

        // Расстояния до всех сторон
        double d_bottom = CalcPointToSegmentDistance(lb, rb);
        double d_right = CalcPointToSegmentDistance(rb, rt);
        double d_top = CalcPointToSegmentDistance(rt, lt);
        double d_left = CalcPointToSegmentDistance(lt, lb);
        return std::min({d_bottom, d_right, d_top, d_left});
    }

    double operator()(const RegularPolygon &poly) const {
        const double distToCenter = (point - poly.center_p).Length();

        // Проверка, находится ли точка внутри описанной окружности
        if (distToCenter <= poly.radius + eps) {
            const Point2D relPoint = point - poly.center_p;
            double pointAngle = std::atan2(relPoint.y, relPoint.x);
            double segmentAngle = 2 * M_PI / poly.sides;

            // Определение сектора многоугольника
            int sector = static_cast<int>(std::floor(pointAngle / segmentAngle + 0.5));
            sector = (sector % poly.sides + poly.sides) % poly.sides;

            // Вычисление углов и координат вершин сектора
            const double angle1 = sector * segmentAngle;
            const double angle2 = (sector + 1) * segmentAngle;
            const Point2D v1 = {poly.radius * std::cos(angle1), poly.radius * std::sin(angle1)};
            const Point2D v2 = {poly.radius * std::cos(angle2), poly.radius * std::sin(angle2)};

            // Проверка положения точки относительно сектора
            const Point2D vec1 = v2 - v1;
            const Point2D vec2 = relPoint - v1;

            const double cross = vec1.Cross(vec2);
            if (cross >= -eps)
                return 0.0;
        }

        // Поиск минимального расстояния до всех сторон
        const std::vector<Point2D> &vertices = poly.Vertices();
        double minDist = std::numeric_limits<double>::max();
        for (int i = 0; i < poly.sides; ++i) {
            const int j = (i + 1) % poly.sides;
            double dist = CalcPointToSegmentDistance(vertices[i], vertices[j]);
            minDist = std::min(minDist, dist);
        }

        return minDist;
    }

    double operator()(const Polygon &poly) const {
        const auto &vertices = poly.Vertices();
        if (IsPointInPolygon(point, vertices))
            return 0.0;  // Точка внутри полигона → расстояние 0

        double minDistance = std::numeric_limits<double>::max();
        const size_t n = vertices.size();

        assert(n >= 3);

        for (size_t i = 0; i < n; ++i) {
            const Point2D &v = vertices[i];
            const Point2D &w = vertices[(i + 1) % n];  // следующая точка, с замыканием
            const double distance = CalcPointToSegmentDistance(v, w);
            minDistance = std::min(minDistance, distance);
        }

        return minDistance;
    }

    double operator()(const auto &shape1) const {
        throw std::logic_error("Unexpected shape for calc distante to point operation");
        return {};
    }

private:
    const double eps = 1e-9;

    double CalcPointToSegmentDistance(const Point2D &a, const Point2D &b) const {
        Point2D ab = b - a;
        Point2D ap = point - a;

        // используем квадрат, так как sqrt 'дорогая' операция
        const double lengthSquared = ab.Dot(ab);
        // Обработка вырожденного случая (точки совпадают)
        if (lengthSquared < eps) {
            return ap.Length();
        }

        if (lengthSquared < eps)
            return ap.Length();  // a и b совпадают

        double t = std::max(0.0, std::min(1.0, ap.Dot(ab) / lengthSquared));
        const Point2D projection = a + ab * t;
        const Point2D distVec = point - projection;
        return distVec.Length();
    }

    // Функция для проверки, находится ли точка внутри полигона
    bool IsPointInPolygon(const Point2D &point, const std::vector<Point2D> &poly) const {
        bool inside = false;
        const size_t n = poly.size();

        for (size_t i = 0, j = n - 1; i < n; j = i++) {
            const Point2D &vi = poly[i];
            const Point2D &vj = poly[j];
            // Проверяем пересечение луча, идущего вправо, с ребром полигона
            if (((vi.y > point.y) != (vj.y > point.y)) &&
                (point.x < (vj.x - vi.x) * (point.y - vi.y) / (vj.y - vi.y) + vi.x)) {
                inside = !inside;
            }
        }
        return inside;
    }
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
    double operator()(const Shape &shape, const Point2D &point) const { return DistanceToPoint(shape, point); }

    double operator()(const Line &line1, const Line &line2) const {
        // Спросить у ревьювера
        // Пришлось добавить CheckIntersection, так как код
        // из PointToShapeDistanceVisitor высчитывает точки,
        // или это не значительно и можно поверить, что точек пересечения нет и использовать одну функцию?
        if (CheckIntersection(line1, line2))
            return 0.0;

        // DRY
        const double d1 = DistanceToPoint(line2, line1.start);
        const double d2 = DistanceToPoint(line2, line1.end);
        const double d3 = DistanceToPoint(line1, line2.start);
        const double d4 = DistanceToPoint(line1, line2.end);
        return std::min({d1, d2, d3, d4});
    }

    double operator()(const Circle &circle1, const Circle &circle2) const {
        const double dist = (circle2.center_p - circle1.center_p).Length();
        if (dist <= fabs(circle1.radius - circle2.radius)) {
            // Одна окружность внутри другой
            return -(std::abs(circle1.radius - circle2.radius) - dist);  // отрицательное значение
        } else if (dist < circle1.radius + circle2.radius) {
            // Окружности пересекаются
            return 0.0;
        }

        // Окружности не пересекаются
        return dist - (circle1.radius + circle2.radius);
    }

    double operator()(const auto &shape1, const auto &shape2) const {
        throw std::logic_error("Unexpected shapes for calc distante betwenn shapes operation");
        return 0.0;
    }

private:
    // Функция для проверки, пересекаются ли два отрезка
    bool CheckIntersection(const Line &line1, const Line &line2) const {
        const double eps = 1e-9;

        const Point2D A = line1.start, B = line1.end;
        const Point2D C = line2.start, D = line2.end;
        const Point2D AB = B - A;
        const Point2D CD = D - C;
        const double cross = AB.Cross(CD);
        const Point2D AC = C - A;
        // Непараллельные отрезки
        if (fabs(cross) > eps) {
            double t = AC.Cross(CD) / cross;
            double u = AC.Cross(AB) / cross;
            if (t >= -eps && t <= 1 + eps && u >= -eps && u <= 1 + eps)
                return true;
            return false;
        }

        // Параллельные отрезки
        double ACxAB = AC.Cross(AB);
        if (fabs(ACxAB) > eps)
            return false;

        // Проверка перекрытия
        const auto [s1_min_x, s1_max_x] = std::minmax(A.x, B.x);
        const auto [s2_min_x, s2_max_x] = std::minmax(C.x, D.x);

        if (s1_max_x < s2_min_x - eps || s2_max_x < s1_min_x - eps)
            return false;

        const auto [s1_min_y, s1_max_y] = std::minmax(A.y, B.y);
        const auto [s2_min_y, s2_max_y] = std::minmax(C.y, D.y);

        if (s1_max_y < s2_min_y - eps || s2_max_y < s1_min_y - eps)
            return false;  // Нет наложения

        return true;
    }
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

}  // namespace geometry::queries