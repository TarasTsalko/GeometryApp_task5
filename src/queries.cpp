#include "queries.hpp"

namespace geometry::queries {

double PointToShapeDistanceVisitor::CalcPointToLineDistance(const Line &line) const {
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

double PointToShapeDistanceVisitor::CalcPointToTriangleDistance(const Triangle &triangle) const {
    using namespace math_utils;
    const Point2D &A = triangle.a;
    const Point2D &B = triangle.b;
    const Point2D &C = triangle.c;

    const double d1 = CrossProduct(point, A, B);
    const double d2 = CrossProduct(point, B, C);
    const double d3 = CrossProduct(point, C, A);

    const bool has_neg = (d1 < -EPSILON) || (d2 < -EPSILON) || (d3 < -EPSILON);
    const bool has_pos = (d1 > EPSILON) || (d2 > EPSILON) || (d3 > EPSILON);

    if (!(has_neg && has_pos)) {
        // Точка внутри треугольника - расстояние равно 0
        return 0.0;
    }

    // 2. Если точка снаружи, вычисляем расстояние до ближайшего ребра
    auto distanceToSegment = [&](Point2D p, Point2D v, Point2D w) {
        const double l2 = (v - w).Dot(v - w);
        if (l2 < EPSILON)
            return (p - v).Dot(p - v);

        const double t = std::max(0.0, std::min(1.0, (p - v).Dot(w - v) / l2));
        Point2D projection = v + (w - v) * t;
        return sqrt((p - projection).Dot(p - projection));
    };

    const double distAB = distanceToSegment(point, A, B);
    const double distBC = distanceToSegment(point, B, C);
    const double distCA = distanceToSegment(point, C, A);

    return std::min({distAB, distBC, distCA});
}

double PointToShapeDistanceVisitor::CalcPointToRectangleDistance(const Rectangle &rectangle) const {
    using namespace math_utils;
    // Угловые точки прямоугольника
    const Point2D &lb = rectangle.GetBottomLeft();
    const Point2D rb = {lb.x + rectangle.GetWidth(), lb.y};
    const Point2D rt = {lb.x + rectangle.GetWidth(), lb.y + rectangle.GetHeight()};
    const Point2D lt = {lb.x, lb.y + rectangle.GetHeight()};

    // Проверка на нахождение внутри прямоугольника
    if (point.x >= lb.x - EPSILON && point.x <= rb.x + EPSILON && point.y >= lb.y - EPSILON &&
        point.y <= lt.y + EPSILON) {
        return 0.0;
    }

    // Расстояния до всех сторон
    const double d_bottom = CalcPointToSegmentDistance(lb, rb);
    const double d_right = CalcPointToSegmentDistance(rb, rt);
    const double d_top = CalcPointToSegmentDistance(rt, lt);
    const double d_left = CalcPointToSegmentDistance(lt, lb);
    return std::min({d_bottom, d_right, d_top, d_left});
}

double PointToShapeDistanceVisitor::CalcPointToCircleDistance(const Circle &circle) const {
    const Point2D dir = point - circle.Center();
    // Получаем расстояние как длину вектора
    const double d = dir.Length();

    // Определяем итоговое расстояние
    if (d > circle.radius) {
        return d - circle.radius;  // Точка снаружи
    } else if (d <= circle.radius) {
        return 0.0;  // Точка внутри или на окружности (уточнил у ревьювера)
    } else {
        std::unreachable();  // без else выдается warning, что не все пути выполнения контролируются
    }
}

double PointToShapeDistanceVisitor::CalcPointToSegmentDistance(const Point2D &a, const Point2D &b) const {
    using namespace math_utils;
    Point2D ab = b - a;
    Point2D ap = point - a;

    // используем квадрат, так как sqrt 'дорогая' операция
    const double lengthSquared = ab.Dot(ab);
    // Обработка вырожденного случая (точки совпадают)
    if (lengthSquared < EPSILON) {
        return ap.Length();
    }

    // Вычисляем параметр t для проекции
    double t = ap.Dot(ab) / lengthSquared;
    // Ограничиваем t диапазоном [0, 1]
    t = std::clamp(t, 0.0, 1.0);

    // Находим проекцию точки на прямую
    const Point2D projection = a + ab * t;
    const Point2D distVec = point - projection;
    return distVec.Length();
}

double PointToShapeDistanceVisitor::CalcPointToRegularPolygonDistance(const RegularPolygon &poly) const {
    using namespace math_utils;
    const double distToCenter = (point - poly.center_p).Length();

    // Проверка, находится ли точка внутри описанной окружности
    if (distToCenter <= poly.radius + EPSILON) {
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
        if (cross >= -EPSILON)
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

double PointToShapeDistanceVisitor::CalcPointToPolygonDistance(const Polygon &poly) const {
    using namespace math_utils;
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

double ShapeToShapeDistanceVisitor::CalcCircleCircleDistance(const Circle &circle1, const Circle &circle2) const {
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

double ShapeToShapeDistanceVisitor::CalcLineLineDistance(const Line &line1, const Line &line2) const {
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

// Функция для проверки, пересекаются ли два отрезка
bool ShapeToShapeDistanceVisitor::CheckIntersection(const Line &line1, const Line &line2) const {
    using namespace math_utils;

    const Point2D A = line1.start, B = line1.end;
    const Point2D C = line2.start, D = line2.end;
    const Point2D AB = B - A;
    const Point2D CD = D - C;
    const double cross = AB.Cross(CD);
    const Point2D AC = C - A;
    // Непараллельные отрезки
    if (fabs(cross) > EPSILON) {
        double t = AC.Cross(CD) / cross;
        double u = AC.Cross(AB) / cross;
        if (t >= -EPSILON && t <= 1 + EPSILON && u >= -EPSILON && u <= 1 + EPSILON)
            return true;
        return false;
    }

    // Параллельные отрезки
    double ACxAB = AC.Cross(AB);
    if (fabs(ACxAB) > EPSILON)
        return false;

    // Проверка перекрытия
    const auto [s1_min_x, s1_max_x] = std::minmax(A.x, B.x);
    const auto [s2_min_x, s2_max_x] = std::minmax(C.x, D.x);

    if (s1_max_x < s2_min_x - EPSILON || s2_max_x < s1_min_x - EPSILON)
        return false;

    const auto [s1_min_y, s1_max_y] = std::minmax(A.y, B.y);
    const auto [s2_min_y, s2_max_y] = std::minmax(C.y, D.y);

    if (s1_max_y < s2_min_y - EPSILON || s2_max_y < s1_min_y - EPSILON)
        return false;  // Нет наложения

    return true;
}

}  // namespace geometry::queries