#pragma once
#include "geometry.hpp"
#include "math_utils.hpp"
#include <algorithm>
#include <cassert>
#include <expected>
#include <format>
#include <limits>
#include <print>
#include <set>
#include <vector>

namespace geometry::triangulation {

struct DelaunayTriangle {
    Point2D a, b, c;

    DelaunayTriangle(Point2D a, Point2D b, Point2D c) : a(a), b(b), c(c) {
        math_utils::SortPointsCounterClockwise(a, b, c);
    }

    bool operator<(const DelaunayTriangle &other) const {
        // Используем std::tie для создания кортежа из вершин треугольника
        return std::tie(a, b, c) < std::tie(other.a, other.b, other.c);
    }

    bool ContainsPoint(const Point2D &p) const {
        using namespace math_utils;
        Point2D center = Circumcenter();
        double radius = Circumradius();
        return center.DistanceTo(p) <= radius + EPSILON;
    }

    Point2D Circumcenter() const {
        double d = 2 * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));
        if (std::abs(d) < 1e-10) {
            return {(a.x + b.x + c.x) / 3, (a.y + b.y + c.y) / 3};
        }

        double ux = ((a.x * a.x + a.y * a.y) * (b.y - c.y) + (b.x * b.x + b.y * b.y) * (c.y - a.y) +
                     (c.x * c.x + c.y * c.y) * (a.y - b.y)) /
                    d;

        double uy = ((a.x * a.x + a.y * a.y) * (c.x - b.x) + (b.x * b.x + b.y * b.y) * (a.x - c.x) +
                     (c.x * c.x + c.y * c.y) * (b.x - a.x)) /
                    d;

        return {ux, uy};
    }

    double Circumradius() const {
        Point2D center = Circumcenter();
        return center.DistanceTo(a);
    }

    std::vector<Point2D> Vertices() const { return {a, b, c}; }

    bool operator==(const DelaunayTriangle &other) const {
        // Поскольку точки уже отсортированы CCW, достаточно проверить прямой порядок
        return (a == other.a && b == other.b && c == other.c);
    }
};

struct Edge {
    Point2D p1, p2;

    Edge(Point2D p1, Point2D p2) : p1(p1), p2(p2) {
        if (p1.x > p2.x || (p1.x == p2.x && p1.y > p2.y)) {
            std::swap(this->p1, this->p2);
        }
    }

    bool operator<(const Edge &other) const {
        using namespace math_utils;
        if (std::abs(p1.x - other.p1.x) > EPSILON)
            return p1.x < other.p1.x;
        if (std::abs(p1.y - other.p1.y) > EPSILON)
            return p1.y < other.p1.y;
        if (std::abs(p2.x - other.p2.x) > EPSILON)
            return p2.x < other.p2.x;
        return p2.y < other.p2.y;
    }

    bool operator==(const Edge &other) const {
        using namespace math_utils;
        return std::abs(p1.x - other.p1.x) < EPSILON && std::abs(p1.y - other.p1.y) < EPSILON &&
               std::abs(p2.x - other.p2.x) < EPSILON && std::abs(p2.y - other.p2.y) < EPSILON;
    }
};

inline GeometryResult<std::vector<DelaunayTriangle>> DelaunayTriangulation(std::span<const Point2D> points) {

    if (points.size() < 3)
        return std::unexpected{GeometryError::InvalidInput};

    // Создание супертреугольника
    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::lowest();
    double max_y = std::numeric_limits<double>::lowest();

    for (const auto &point : points) {
        min_x = std::min(min_x, point.x);
        min_y = std::min(min_y, point.y);
        max_x = std::max(max_x, point.x);
        max_y = std::max(max_y, point.y);
    }

    // Проверка на коллинеарность всех точек
    if (max_x == min_x || max_y == min_y)
        return std::unexpected{GeometryError::DegenrateCase};

    double dx = max_x - min_x;
    double dy = max_y - min_y;
    double delta_max = std::max(dx, dy);
    double mid_x = (min_x + max_x) / 2.0;
    double mid_y = (min_y + max_y) / 2.0;

    Point2D p1(mid_x - 2 * delta_max, mid_y - delta_max);
    Point2D p2(mid_x, mid_y + 2 * delta_max);
    Point2D p3(mid_x + 2 * delta_max, mid_y - delta_max);

    DelaunayTriangle super_triangle(p1, p2, p3);
    std::vector<DelaunayTriangle> triangles = {super_triangle};

    // Оптимизированный основной цикл
    for (const auto &point : points) {
        std::vector<DelaunayTriangle> bad_triangles;

        // Используем std::copy_if для фильтрации
        std::copy_if(triangles.begin(), triangles.end(), std::back_inserter(bad_triangles),
                     [&point](const DelaunayTriangle &t) { return t.ContainsPoint(point); });

        std::set<Edge> all_edges;
        std::set<Edge> shared_edges;

        // Собираем все рёбра и общие рёбра
        for (const auto &triangle : bad_triangles) {
            for (const auto &edge :
                 {Edge(triangle.a, triangle.b), Edge(triangle.b, triangle.c), Edge(triangle.c, triangle.a)}) {
                if (all_edges.count(edge)) {
                    shared_edges.insert(edge);
                }
                all_edges.insert(edge);
            }
        }

        // Находим граничные рёбра
        std::set<Edge> polygon_edges;
        std::set_difference(all_edges.begin(), all_edges.end(), shared_edges.begin(), shared_edges.end(),
                            std::inserter(polygon_edges, polygon_edges.begin()));

        // Удаляем плохие треугольники
        triangles.erase(std::remove_if(triangles.begin(), triangles.end(),
                                       [&bad_triangles](const DelaunayTriangle &t) {
                                           return std::find(bad_triangles.begin(), bad_triangles.end(), t) !=
                                                  bad_triangles.end();
                                       }),
                        triangles.end());

        // Создаём новые треугольники
        for (const auto &edge : polygon_edges) {
            triangles.emplace_back(edge.p1, edge.p2, point);
        }
    }

    // Удаляем треугольники, связанные с вершинами супертреугольника
    triangles.erase(
        std::remove_if(triangles.begin(), triangles.end(),
                       [&super_triangle](const DelaunayTriangle &t) {
                           const auto &t_vertices = t.Vertices();
                           const auto &super_vertices = super_triangle.Vertices();
                           return std::ranges::any_of(super_vertices, [&t_vertices](const Point2D &super_vertex) {
                               return std::ranges::any_of(t_vertices, [&super_vertex](const Point2D &t_vertex) {
                                   return super_vertex == t_vertex;
                               });
                           });
                       }),
        triangles.end());
    return triangles;
}

}  // namespace geometry::triangulation

template <>
struct std::formatter<geometry::triangulation::DelaunayTriangle> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::triangulation::DelaunayTriangle &t, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "DelaunayTriangle({}, {}, {})", t.a, t.b, t.c);
    }
};
