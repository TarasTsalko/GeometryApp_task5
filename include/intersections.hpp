#pragma once
#include "geometry.hpp"
#include <cmath>
#include <optional>
#include <print>
#include <variant>
#include <vector>

namespace geometry::intersections {

/*
 * Класс для поиска пересечений между двумя фигурами
 *
 * Требуется организовать возможность нахождения пересечений только для следующих комбинаций фигур:
 *    - Line   & Line
 *    - Line   & Circle
 *    - Circle & Circle
 *
 * Для всех остальных требуется выбросить исключение std::logic_error
 */
class IntersectionVisitor {
public:
    std::vector<Point2D> operator()(const geometry::Line &line1, const geometry::Line &line2) const {
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
                return {Point2D{A.x + t * AB.x, A.y + t * AB.y}};
            return {};
        }

        // Параллельные отрезки
        double ACxAB = AC.Cross(AB);
        if (fabs(ACxAB) > eps)
            return {};

        // Проверка полного совпадения отрезков
        if ((A == C && B == D) || (A == D && B == C)) {
            return {A, B};  // Возвращаем весь отрезок
        }

        // Проверка перекрытия
        const auto [s1_min_x, s1_max_x] = std::minmax(A.x, B.x);
        const auto [s2_min_x, s2_max_x] = std::minmax(C.x, D.x);

        if (s1_max_x < s2_min_x - eps || s2_max_x < s1_min_x - eps)
            return {};

        const auto [s1_min_y, s1_max_y] = std::minmax(A.y, B.y);
        const auto [s2_min_y, s2_max_y] = std::minmax(C.y, D.y);

        if (s1_max_y < s2_min_y - eps || s2_max_y < s1_min_y - eps)
            return {};  // Нет наложения

        const Point2D intersection_start = {std::max(s1_min_x, s2_min_x), std::max(s1_min_y, s2_min_y)};
        const Point2D intersection_end = {std::min(s1_max_x, s2_max_x), std::min(s1_max_y, s2_max_y)};
        // если отрезки имеют общую точку (в этом случаи intersection_start == intersection_end )
        if ((intersection_end - intersection_start).Length() <= 1e-10)
            return {intersection_start};
        return {intersection_start, intersection_end};
    }

    std::vector<Point2D> operator()(const geometry::Circle &circle1, const geometry::Circle &circle2) const {
        const Point2D dir = circle2.center_p - circle1.center_p;
        const double dist = dir.Length();

        // нет пересечений
        if (dist > circle1.radius + circle2.radius)
            return {};

        // одна окружность внутри другой
        if (dist < fabs(circle1.radius - circle2.radius)) {
            // возвращаем центр меньшей окружности
            // уточнить у ревьювера, что предпочтительно возвращать в данной ситуации
            return {circle1.radius > circle2.radius ? circle2.Center() : circle1.Center()};
        }
        // если окружности совподают, то это можно трактовать
        // как частный случай пересечения и вернуть все точки
        if (dist == 0.0 && circle1.radius == circle2.radius) {
            // уточнить у ревьювера, что предпочтительно возвращать в данной ситуации
            return {circle1.Center()};  // Окружности совпадают (для удобства возвращаем центральную точку)
        }

        // расстояния от центра первой окружности до точки, которая лежит на линии, соединяющей центры двух окружностей,
        // и которая делит это расстояние в соответствии с радиусами окружностей.
        const double a = (circle1.radius * circle1.radius - circle2.radius * circle2.radius + dist * dist) / (2 * dist);

        // Точка на линии между центрами
        const double x0 = circle1.center_p.x + (a * dir.x) / dist;
        const double y0 = circle1.center_p.y + (a * dir.y) / dist;

        // Случай касания (внутреннего или внешнего)
        if (dist == circle1.radius + circle2.radius || dist == fabs(circle1.radius - circle2.radius))
            return {Point2D{x0, y0}};

        // Случай двух точек пересечения
        // h - используется для нахождения расстояния от точки, которая делит расстояние между центрами двух окружностей
        // до точки пересечения (или касания) окружностей, перпендикулярно линии, соединяющей центры окружностей.
        const double h = sqrt(circle1.radius * circle1.radius - a * a);
        const double rx = -dir.y * (h / dist);
        const double ry = dir.x * (h / dist);

        return {Point2D{x0 + rx, y0 + ry}, Point2D{x0 - rx, y0 - ry}};
    }

    std::vector<Point2D> operator()(const geometry::Circle &circle, const geometry::Line &line) const {
        return FindLineCircleIntersectionImpl(circle, line);
    }

    std::vector<Point2D> operator()(const geometry::Line &line, const geometry::Circle &circle) const {
        return FindLineCircleIntersectionImpl(circle, line);
    }

    std::vector<Point2D> operator()(const auto &shape1, const auto &shape2) const {
        throw std::logic_error("Unexpected shapes for intersection operation");
        return {};
    }

private:
    std::vector<Point2D> FindLineCircleIntersectionImpl(const geometry::Circle &circle,
                                                        const geometry::Line &line) const {
        const double eps = 1e-9;

        // Вектор направления отрезка
        const Point2D dir = line.end - line.start;
        // Векторы от центра окружности до концов отрезка
        const Point2D centerToStart = line.start - circle.center_p;
        const Point2D centerToEnd = line.end - circle.center_p;

        // Проверяем, находятся ли концы отрезка внутри окружности
        bool startInside = centerToStart.Dot(centerToStart) < circle.radius * circle.radius + eps;
        bool endInside = centerToEnd.Dot(centerToEnd) < circle.radius * circle.radius + eps;

        // Если оба конца внутри, проверяем ближайшую точку
        if (startInside && endInside) {
            // Находим параметр t для ближайшей точки
            double t = centerToStart.Dot(dir) / dir.Dot(dir);
            t = std::max(0.0, std::min(1.0, t));

            Point2D closestPoint = line.start + dir * t;
            if ((closestPoint - circle.center_p).Dot(closestPoint - circle.center_p) <
                circle.radius * circle.radius + eps) {
                // Отрезок полностью внутри - возвращаем обе точки
                // спросить у ревьювера? xnj kexit для единообразия
                return {line.start, line.end};
            }
        }

        // Коэффициенты квадратного уравнения
        const double a = dir.Dot(dir);                // a = |d|^2
        const double b = 2 * centerToStart.Dot(dir);  // b = 2 * centerToStart * d
        const double c =
            centerToStart.Dot(centerToStart) - circle.radius * circle.radius;  // c = |centerToStart|^2 - r^2

        // Вычисление дискриминанта
        double discriminant = b * b - 4 * a * c;
        if (discriminant < -eps)
            return {};

        // Вычисляем корень дискриминанта
        discriminant = std::sqrt(std::max(0.0, discriminant));
        // Находим оба решения (параметры t вдоль отрезка)
        const double t1 = (-b - discriminant) / (2 * a);
        const double t2 = (-b + discriminant) / (2 * a);

        std::vector<Point2D> points;
        // Проверяем, находится ли t1 в пределах отрезка [0, 1]
        if (t1 >= -eps && t1 <= 1.0 + eps)
            points.push_back(line.start + dir * t1);

        // Проверяем, находится ли t2 в пределах отрезка [0, 1]
        if (t2 >= -eps && t2 <= 1.0 + eps)
            if (discriminant > eps || points.empty())
                points.push_back(line.start + dir * t2);

        return points;
    }
};

inline std::optional<std::vector<Point2D>> GetIntersectPoints(const Shape &shape1, const Shape &shape2) {
    const auto res = std::visit(IntersectionVisitor{}, shape1, shape2);
    if (!res.empty())
        return res;
    return std::nullopt;
}

}  // namespace geometry::intersections