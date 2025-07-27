#include "intersections.hpp"
#include "math_utils.hpp"
#include <cassert>
#include <cmath>
#include <utility>

namespace geometry::intersections {

using namespace math_utils;

IntersectionResult IntersectionVisitor::FindLineCircleIntersectionImpl(const geometry::Circle &circle,
                                                                       const geometry::Line &line) const {
    // Вектор направления отрезка
    const Point2D dir = line.end - line.start;
    // Векторы от центра окружности до концов отрезка
    const Point2D centerToStart = line.start - circle.center_p;
    const Point2D centerToEnd = line.end - circle.center_p;

    std::vector<Point2D> points;
    IntersectionType type = IntersectionType::None;

    // Проверяем, находятся ли концы отрезка внутри окружности
    bool startInside = centerToStart.Dot(centerToStart) < circle.radius * circle.radius + EPSILON;
    bool endInside = centerToEnd.Dot(centerToEnd) < circle.radius * circle.radius + EPSILON;

    // Если оба конца внутри, проверяем ближайшую точку
    if (startInside && endInside) {
        // Находим параметр t для ближайшей точки
        double t = centerToStart.Dot(dir) / dir.Dot(dir);
        t = std::max(0.0, std::min(1.0, t));

        Point2D closestPoint = line.start + dir * t;
        if ((closestPoint - circle.center_p).Dot(closestPoint - circle.center_p) <
            circle.radius * circle.radius + EPSILON) {
            // Отрезок полностью внутри, пересечений нет ( уточнил у ревьювера )
            type = IntersectionType::None;
            return {type, points};
        }
    }

    // Коэффициенты квадратного уравнения
    const double a = dir.Dot(dir);                                                      // a = |d|^2
    const double b = 2 * centerToStart.Dot(dir);                                        // b = 2 * centerToStart * d
    const double c = centerToStart.Dot(centerToStart) - circle.radius * circle.radius;  // c = |centerToStart|^2 - r^2

    // Вычисление дискриминанта
    double discriminant = b * b - 4 * a * c;
    if (discriminant < -EPSILON)
        return {IntersectionType::None, points};

    // Вычисляем корень дискриминанта
    discriminant = std::sqrt(std::max(0.0, discriminant));
    // Находим оба решения (параметры t вдоль отрезка)
    const double t1 = (-b - discriminant) / (2 * a);
    const double t2 = (-b + discriminant) / (2 * a);

    /*
    const Circle circle({0.0, 0.0}, 3.0);
    const Line line({5.0, 5.0}, {6.0, 6.0});

    В этом случае:
    * Существует пересечение между бесконечной прямой, проходящей через точки отрезка, и окружностью
    * Однако пересечения с самим отрезком нет, поскольку параметры t для точек пересечения:
    - t1 ~ -7.12132
    - t2 ~ -2.87868

    * Значения t находятся вне диапазона [0, 1], что означает расположение точек пересечения за пределами отрезка
    * Если убрать проверки:
    - if (t1 >= -EPSILON && t1 <= 1.0 + EPSILON)
    - или if (t2 >= -EPSILON && t2 <= 1.0 + EPSILON)
    то алгоритм ошибочно определит наличие пересечения с отрезком

    Таким образом, данные проверки необходимы для корректного определения пересечения именно с отрезком, а не с
    бесконечной прямой.
    */

    // Проверяем, находится ли t1 в пределах отрезка [0, 1]
    if (t1 >= -EPSILON && t1 <= 1.0 + EPSILON)
        points.push_back(line.start + dir * t1);

    // Проверяем, находится ли t2 в пределах отрезка [0, 1]
    if (t2 >= -EPSILON && t2 <= 1.0 + EPSILON)
        if (discriminant > EPSILON || points.empty())
            points.push_back(line.start + dir * t2);

    // Определяем тип пересечения
    if (points.empty()) {
        type = IntersectionType::None;
    } else if (points.size() == 1) {
        type = IntersectionType::Point;
    } else if (points.size() == 2) {
        type = IntersectionType::Segment;
    } else {
        std::unreachable();
    }

    return {type, points};
}

IntersectionResult IntersectionVisitor::FindLineLineleIntersectionImpl(const geometry::Line &line1,
                                                                       const geometry::Line &line2) const {
    const Point2D A = line1.start, B = line1.end;
    const Point2D C = line2.start, D = line2.end;
    const Point2D AB = B - A;
    const Point2D CD = D - C;
    const double cross = AB.Cross(CD);
    const Point2D AC = C - A;
    IntersectionType type = IntersectionType::None;
    std::vector<Point2D> points;

    // Непараллельные отрезки
    if (fabs(cross) > EPSILON) {
        const double t = AC.Cross(CD) / cross;
        const double u = AC.Cross(AB) / cross;
        if (t >= -EPSILON && t <= 1 + EPSILON && u >= -EPSILON && u <= 1 + EPSILON) {
            points.push_back({A.x + t * AB.x, A.y + t * AB.y});
            type = IntersectionType::Point;
            return {type, points};
        }
        return {IntersectionType::None, points};
    }

    // Параллельные отрезки
    const double ACxAB = AC.Cross(AB);
    if (fabs(ACxAB) > EPSILON) {
        return {IntersectionType::Collinear, points};
    }

    // Проверка полного совпадения отрезков
    // Переписал operator== для Point2D с учетом точности EPSILON из math_utils
    if ((A == C && B == D) || (A == D && B == C)) {
        points.push_back(A);
        points.push_back(B);
        type = IntersectionType::Coincident;
        return {type, points};
    }

    // Проверка перекрытия
    const auto [s1_min_x, s1_max_x] = std::minmax(A.x, B.x);
    const auto [s2_min_x, s2_max_x] = std::minmax(C.x, D.x);

    if (s1_max_x < s2_min_x - EPSILON || s2_max_x < s1_min_x - EPSILON)
        return {IntersectionType::None, points};

    const auto [s1_min_y, s1_max_y] = std::minmax(A.y, B.y);
    const auto [s2_min_y, s2_max_y] = std::minmax(C.y, D.y);

    if (s1_max_y < s2_min_y - EPSILON || s2_max_y < s1_min_y - EPSILON)
        return {IntersectionType::None, points};  // Нет наложения

    Point2D intersection_start = {std::max(s1_min_x, s2_min_x), std::max(s1_min_y, s2_min_y)};
    Point2D intersection_end = {std::min(s1_max_x, s2_max_x), std::min(s1_max_y, s2_max_y)};

    // если отрезки имеют общую точку
    if ((intersection_end - intersection_start).Length() < EPSILON) {
        points.push_back(intersection_start);
        type = IntersectionType::Point;
    } else {
        points.push_back(intersection_start);
        points.push_back(intersection_end);
        type = IntersectionType::Segment;
    }

    return {type, points};
}

IntersectionResult IntersectionVisitor::FindCircleCircleIntersectionImpl(const geometry::Circle &circle1,
                                                                         const geometry::Circle &circle2) const {
    const Point2D dir = circle2.center_p - circle1.center_p;
    const double dist = dir.Length();
    IntersectionType type = IntersectionType::None;
    std::vector<Point2D> points;

    // нет пересечений
    if (dist > circle1.radius + circle2.radius)
        return {IntersectionType::None, points};

    // одна окружность внутри другой
    if (dist < fabs(circle1.radius - circle2.radius)) {
        // возвращаем центр меньшей окружности
        return {IntersectionType::None, points};
    }

    // если окружности совпадают
    if (dist == 0.0 && circle1.radius == circle2.radius) {
        points.push_back(circle1.Center());
        type = IntersectionType::Coincident;
        return {type, points};
    }

    // расстояния от центра первой окружности до точки, которая лежит на линии, соединяющей центры двух окружностей
    const double a = (circle1.radius * circle1.radius - circle2.radius * circle2.radius + dist * dist) / (2 * dist);

    // Точка на линии между центрами
    const double x0 = circle1.center_p.x + (a * dir.x) / dist;
    const double y0 = circle1.center_p.y + (a * dir.y) / dist;

    // Случай касания (внутреннего или внешнего)
    if (fabs(dist - (circle1.radius + circle2.radius)) < EPSILON ||
        fabs(dist - (circle1.radius - circle2.radius)) < EPSILON) {
        points.push_back(Point2D{x0, y0});
        type = IntersectionType::Point;
        return {type, points};
    }

    // Случай двух точек пересечения
    const double h = sqrt(circle1.radius * circle1.radius - a * a);
    const double rx = -dir.y * (h / dist);
    const double ry = dir.x * (h / dist);

    points.push_back(Point2D{x0 + rx, y0 + ry});
    points.push_back(Point2D{x0 - rx, y0 - ry});
    type = IntersectionType::Segment;

    return {type, points};
}

}  // namespace geometry::intersections