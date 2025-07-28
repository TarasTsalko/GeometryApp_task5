#pragma once
#include "geometry.hpp"
#include "math_utils.hpp"
#include <optional>
#include <print>
#include <variant>
#include <vector>

namespace geometry::intersections {

enum class IntersectionType {
    None,        // Нет пересечения
    Point,       // Одна точка пересечения
    Segment,     // Отрезок пересечения (частичное перекрытие)
    Coincident,  // Полное совпадение отрезков
    Collinear    // Коллинеарны, но не пересекаются
};

struct IntersectionResult {
    IntersectionType type;
    std::vector<Point2D> points;
};

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
    [[nodiscard]] IntersectionResult operator()(const geometry::Line &line1, const geometry::Line &line2) const {
        return FindLineLineleIntersectionImpl(line1, line2);
    }

    [[nodiscard]] IntersectionResult operator()(const geometry::Circle &circle1,
                                                const geometry::Circle &circle2) const {
        return FindCircleCircleIntersectionImpl(circle1, circle2);
    }

    [[nodiscard]] IntersectionResult operator()(const geometry::Circle &circle, const geometry::Line &line) const {
        return FindLineCircleIntersectionImpl(circle, line);
    }

    [[nodiscard]] IntersectionResult operator()(const geometry::Line &line, const geometry::Circle &circle) const {
        return FindLineCircleIntersectionImpl(circle, line);
    }

    [[nodiscard]] IntersectionResult operator()(const auto &shape1, const auto &shape2) const {
        throw std::logic_error("Unexpected shapes for intersection operation");
        return {};
    }

private:
    [[nodiscard]] IntersectionResult FindLineCircleIntersectionImpl(const geometry::Circle &circle,
                                                                    const geometry::Line &line) const;
    [[nodiscard]] IntersectionResult FindLineLineleIntersectionImpl(const geometry::Line &line1,
                                                                    const geometry::Line &line2) const;
    [[nodiscard]] IntersectionResult FindCircleCircleIntersectionImpl(const geometry::Circle &circle1,
                                                                      const geometry::Circle &circle2) const;
};

// При наличии IntersectionResult std::optional уже не нужен и можно возвращать просто IntersectionResult,
// но так как в main по заданию нужно продемонстрировать монадический интерфейс, то в случае
// IntersectionType::None или IntersectionType::Collinear возвращаем std::nullopt
inline std::optional<IntersectionResult> GetIntersectPoints(const Shape &shape1, const Shape &shape2) {
    const auto res = std::visit(IntersectionVisitor{}, shape1, shape2);
    if (res.type == IntersectionType::None || res.type == IntersectionType::Collinear)
        return std::nullopt;
    return res;
}

}  // namespace geometry::intersections