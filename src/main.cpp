#include "convex_hull.hpp"
#include "geometry.hpp"
#include "intersections.hpp"
#include "queries.hpp"
#include "shape_utils.hpp"
#include "triangulation.hpp"
#include "visualization.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <print>
#include <ranges>
#include <span>
#include <utility>
#include <variant>
#include <vector>

using namespace geometry;

namespace rng = std::ranges;
namespace views = std::ranges::views;

template <class... Ts>
struct Multilambda : Ts... {
    using Ts::operator()...;
};

auto GetPrinter() {

    Multilambda printer{[](const Line &line) { return std::format("{}", line); },
                        [](const Triangle &tri) { return std::format("{}", tri); },
                        [](const Circle &circle) { return std::format("{}", circle); },
                        [](const Rectangle &rect) { return std::format("{}", rect); },
                        [](const RegularPolygon &poly) { return std::format("{}", poly); },
                        [](const Polygon &poly) { return std::format("{}", poly); },
                        [](const auto &shape) {
                            std::unreachable();
                            return std::string("");
                        }};
    return printer;
}

void PrintEachShapeHeight(const std::vector<Shape> &shapes) {
    using namespace geometry::queries;

    rng::for_each(shapes | std::views::enumerate, [](const auto &pair) {
        const auto &[index, shape] = pair;
        std::println("Shape index = {}, height = {}", index, queries::GetHeight(shape));
    });
}

void PrintAllIntersections(const Shape &shape, const std::vector<Shape> &others) {
    std::println("\n=== Intersections ===");

    using namespace geometry::intersections;
    Multilambda operationChecker{[](const Line &line) { return true; }, [](const Circle &circle) { return true; },
                                 [](const auto &shape) { return false; }};

    auto filteredShapes =
        others | views::filter([&operationChecker](const Shape &shape) { return std::visit(operationChecker, shape); });

    const auto printer = GetPrinter();
    rng::for_each(filteredShapes, [&](const auto &otherShape) {
        GetIntersectPoints(shape, otherShape)
            .or_else([&]() -> std::optional<std::vector<geometry::Point2D>> {
                std::println("Фигура {} и фигура {} не пересекаются", std::visit(printer, shape),
                             std::visit(printer, otherShape));
                return std::nullopt;
            })
            .and_then([&](const std::vector<geometry::Point2D> &points)
                          -> std::optional<std::vector<geometry::Point2D>> {  // Важно указать возвращаемый тип
                std::println("Фигура {} и фигура {} пересекаются в точках: {}", std::visit(printer, shape),
                             std::visit(printer, otherShape), points);
                return std::nullopt;  // Возвращаем std::nullopt типа std::optional<void>
            });
    });
}

void PrintDistancesFromPointToShapes(Point2D p, const std::vector<Shape> &shapes) {

    using namespace geometry::queries;
    std::println("\n=== Distance from Point Test ===");
    std::println("Testing point: {} ", p);

    std::vector<Shape> result(5);
    std::random_device rd;
    std::mt19937 gen(rd());

    // Если элементов меньше 5, берём все
    const size_t count = std::min(5ul, shapes.size());
    rng::sample(shapes, result.begin(), count, gen);

    auto printDistance = [&p](const auto &shape) {
        const double dist = queries::DistanceToPoint(shape, p);
        std::println("Расстояние от точки {} до фигуры {} равно {}", p, std::visit(GetPrinter(), shape), dist);
    };

    std::ranges::for_each(result, printDistance);
}

void PerformShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Analysis ===");

    auto collisions = geometry::utils::FindAllCollisions(shapes);
    assert(collisions.size() > 0);
    const auto printer = GetPrinter();
    std::ranges::for_each(collisions, [&printer](const auto &collision) {
        std::println("{} и {} пересекаются", std::visit(printer, collision.first),
                     std::visit(printer, collision.second));
    });

    std::println("=============================================================================");
    const auto highestShapeIndex = geometry::utils::FindHighestShape(shapes);
    assert(highestShapeIndex.has_value());
    // уточнил у наставника (под высотой в данной задаче понимается, то где фигура распроржена на графике)
    // значит моксимальной ('самой высокой') будет фигура, расположенная выше всего по оси Y
    const size_t index = highestShapeIndex.value();
    std::println("Фигура расположенная выше всех {}, index = {} ", std::visit(printer, shapes[index]), index);

    std::println("=============================================================================");

    Multilambda operationChecker{[](const Line &line) { return true; }, [](const Circle &circle) { return true; },
                                 [](const auto &shape) { return false; }};

    auto filteredShapes =
        shapes | views::filter([&operationChecker](const Shape &shape) { return std::visit(operationChecker, shape); });

    const size_t s = rng::distance(filteredShapes);
    if (s >= 2) {
        // Выводим расстояние между первыми двумя подходящими телами
        const auto &shape0 = *filteredShapes.begin();
        const auto &shape1 = *std::next(filteredShapes.begin());
        const auto res = queries::DistanceBetweenShapes(shape0, shape1);
        assert(res.has_value());
        std::println("Расстояние между {} и {} равно {}", std::visit(printer, shape0), std::visit(printer, shape1),
                     res.value());
    }
}

void PerformExtraShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Extra Analysis ===");

    // Получаем принтер один раз
    const auto printer = GetPrinter();

    // Создаем диапазон из исходного контейнера shapes
    auto filteredShapes = shapes |
                          // Фильтруем фигуры по высоте
                          std::views::filter([&](const auto &shape) { return queries::GetHeight(shape) >= 50.0; }) |
                          // Берем первые 3 элемента
                          std::views::take(3) |
                          // Преобразуем через std::visit
                          std::views::transform([&](const auto &shape) { return std::visit(printer, shape); });

    // Выводим результаты через for_each
    std::ranges::for_each(filteredShapes, [](const auto &printedShape) {
        std::println("Фигура, находящаяся выше 50: {}", printedShape);
    });

    const auto minHeightShape =
        std::ranges::min(shapes, std::ranges::less{}, [](const auto &shape) { return queries::GetHeight(shape); });

    const auto maxHeightShape =
        std::ranges::max(shapes, std::ranges::less{}, [](const auto &shape) { return queries::GetHeight(shape); });

    std::println("Фигура c наименьшей высотой {}, высота = {} ", std::visit(printer, minHeightShape),
                 queries::GetHeight(minHeightShape));

    std::println("Фигура c найбольшей высотой {}, высота = {} ", std::visit(printer, maxHeightShape),
                 queries::GetHeight(maxHeightShape));

    /*
     * Используйте ranges и созданные классы чтобы:
     *     - Вывести 3 любые фигуры, которые находятся выше 50.0
     *     - Вывести фигуры с наименьшей и с наибольшей высотами
     */
}

std::vector<Point2D> ColllectAllPoints(std::span<const Shape> shapes) {

    std::vector<Point2D> allPoints;
    rng::for_each(shapes, [&allPoints](const auto &shape) {
        std::visit(Multilambda{[&](const auto &s) -> void {
                                   const auto points = s.Vertices();
                                   allPoints.reserve(allPoints.size() + points.size());
                                   rng::copy(points, std::back_inserter(allPoints));
                               },
                               [&](const Shape &) -> void { std::unreachable(); }},
                   shape);
    });
    return allPoints;
}

int main() {
    utils::ShapeGenerator generator(-50.0, 50.0, 5.0, 25.0);
    std::vector<Shape> shapes = generator.GenerateShapes(15);

    std::println("Generated {} random shapes", shapes.size());

    // Выведите индекс каждой фигуры и её высоту
    PrintEachShapeHeight(shapes);

    //
    // Вызываем разработанные функции
    //

    PrintAllIntersections(shapes[0], shapes);

    PrintDistancesFromPointToShapes(Point2D{10.0, 10.0}, shapes);

    PerformShapeAnalysis(shapes);

    PerformExtraShapeAnalysis(shapes);

    //
    // Рисуем все фигуры
    //
    // Важно: после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 2ой график
    //
    geometry::visualization::Draw(shapes);

    //
    // Формируем список из вершин всех фигур
    //
    std::vector<Point2D> points = ColllectAllPoints(shapes);
    const auto polygonPoints = geometry::convex_hull::GrahamScan(points);
    if (!polygonPoints) {
        std::println("GrahamScan error: {}", geometry::ConvertGeometryError(polygonPoints.error()));
        return -1;
    }

    geometry::Polygon poly(polygonPoints.value());
    shapes.push_back(poly);
    geometry::visualization::Draw(shapes);

    //
    // Находим список точек, для построения выпуклой оболочки - convex hull - алгоритмом Грэхема
    // Создаём из них объект класса `Polygon` и добавляем его в список shapes
    // Рисуем все фигуры
    //

    //
    // после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 3ий график
    //

    {
        std::vector<Point2D> points = {{0, 0}, {10, 0}, {5, 8}, {15, 5}, {2, 12}};

        const auto triangulation = geometry::triangulation::DelaunayTriangulation(points);
        if (!triangulation) {
            std::println("Triangulation error: {}", geometry::ConvertGeometryError(triangulation.error()));
            return -2;
        }
        geometry::visualization::Draw(triangulation.value());
        std::vector<Point2D> polygonPoints;
        polygonPoints.reserve(triangulation.value().size() * 3);

        for (const auto &triangle : triangulation.value()) {
            const auto &vertices = triangle.Vertices();
            std::ranges::copy(vertices, std::back_inserter(polygonPoints));
        }

        std::println("Точки треангуляции = {:new_line}", polygonPoints);
        //
        // Используйте список точек points или свой, чтобы
        // выполнить алгоритм триангуляции Делоне алгоритмом Боуэра-Ватсона
        //
        // После успешного завершения алгоритма - выведите результат для проверки
        // используя geometry::visualization::Draw
        //
    }
    return 0;
}