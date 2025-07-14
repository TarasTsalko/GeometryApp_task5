#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <expected>
#include <flat_map>
#include <format>
#include <limits>
#include <numbers>
#include <print>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace geometry {

/*
 * Добавьте к методам класса Point2D и Lines2DDyn все необходимые аттрибуты и спецификаторы
 * Важно: Возвращаемый тип и принимаемые аргументы менять не нужно
 */
struct Point2D {
    double x, y;

    constexpr Point2D() : x(0), y(0) {}
    constexpr Point2D(double x, double y) : x(x), y(y) {}

    // Comparison
    bool operator<(const Point2D &other) const noexcept { return x < other.x && y < other.y; }
    bool operator==(const Point2D &other) const noexcept { return x == other.x && y == other.y; }

    // Binary math operators
    [[nodiscard]] Point2D operator+(const Point2D &other) const noexcept { return {x + other.x, y + other.y}; }
    // операторы должны быть const и noexcept, так как методы в которых они вызываются const
    // и noexcept
    [[nodiscard]] Point2D operator-(const Point2D &other) const noexcept { return {x - other.x, y - other.y}; }
    [[nodiscard]] Point2D operator*(double value) const noexcept { return {x * value, y * value}; }
    [[nodiscard]] Point2D operator/(double value) const { return {x / value, y / value}; }

    // Binary geometry operations
    [[nodiscard]] double Dot(const Point2D &other) const noexcept { return x * other.x + y * other.y; }
    [[nodiscard]] double Cross(const Point2D &other) const noexcept { return x * other.y - y * other.x; }
    [[nodiscard]] double Length() const noexcept { return std::sqrt(x * x + y * y); }
    [[nodiscard]] double DistanceTo(const Point2D &other) const noexcept { return (*this - other).Length(); }

    Point2D Normalize() {
        const double len = Length();
        return len > 0 ? Point2D{x / len, y / len} : Point2D{0, 0};
    }
};

template <size_t N>
struct Lines2D {
    std::array<double, N> x;
    std::array<double, N> y;
};

struct Lines2DDyn {
    std::vector<double> x;
    std::vector<double> y;

    void Reserve(size_t n) {
        x.reserve(n);
        y.reserve(n);
    }
    void PushBack(Point2D p) {
        x.push_back(p.x);
        y.push_back(p.y);
    }
    void PushBack(double px, double py) {
        x.push_back(px);
        y.push_back(py);
    }
    [[nodiscard]] Point2D Front() const noexcept { return {x.front(), y.front()}; }
};

struct BoundingBox {
    double min_x, min_y, max_x, max_y;

    /* ваш код здесь */
    [[nodiscard]] double Width() const noexcept { return max_x - min_x; }

    [[nodiscard]] double Height() const noexcept { return max_y - min_y; }

    [[nodiscard]] Point2D Center() const noexcept { return {(min_x + max_x) / 2.0, (min_y + max_y) / 2.0}; }

    [[nodiscard]] bool Overlaps(const BoundingBox &box) const noexcept {
        return !(max_x < box.min_x || min_x > box.max_x || max_y < box.min_y || min_y > box.max_y);
    };

    // Улучшенная реализация оператора сравнения
    bool operator==(const BoundingBox &other) const noexcept {
        const double eps = 1e-10;
        return std::abs(min_x - other.min_x) < eps && std::abs(min_y - other.min_y) < eps &&
               std::abs(max_x - other.max_x) < eps && std::abs(max_y - other.max_y) < eps;
    }
};

struct Line {
    Point2D start, end;

    /* ваш код здесь */
    // Вопрос к ревьюверу, так как в описании к заданию
    // данный класс должен иметь метод Length, то с математической точки зрения это отрезок?
    // подрузомевается, что насчитывается дленна отрезка?
    [[nodiscard]] double Length() const noexcept { return (end - start).Length(); }

    [[nodiscard]] Point2D Direction() const noexcept { return end - start; }

    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        BoundingBox bb;
        bb.min_x = std::min(start.x, end.x);
        bb.min_y = std::min(start.y, end.y);
        bb.max_x = std::max(start.x, end.x);
        bb.max_y = std::max(start.y, end.y);
        return bb;
    }

    // нужно уточнить у ревьювер значение метода Height,
    // пока предположим, что это высота её BoundBox
    // высота отрезка, всегда совподает с высотой охватываюшего его BoundingBox-а

    // Уточнил у наставника, в п7. задания нужно найти фигуру выше всего расположенную
    // на графике, а не наивысщую, соответственно метод Height должен возвращать максимальное
    // значение (с ечетом знака) y-координату
    [[nodiscard]] double Height() const noexcept { return std::max(start.y, end.y); }

    [[nodiscard]] Point2D Center() const noexcept { return {(start.x + end.x) / 2.0, (start.y + end.y) / 2.0}; }

    [[nodiscard]] std::array<Point2D, 2u> Vertices() const noexcept { return std::array{start, end}; }

    [[nodiscard]] Lines2D<2> Lines() const noexcept {
        Lines2D<2> res;
        res.x = {start.x, end.x};
        res.y = {start.y, end.y};
        return res;
    }
};

struct Triangle {
    Point2D a, b, c;

    /* ваш код здесь */
    [[nodiscard]] double Area() const noexcept {
        auto ab = b - a;
        auto ac = c - a;
        // 1\2 длинны векторного произведения
        // так же как и с координатами, но блогадаря перегрузкам
        // не нужно работать с координатами на прямую
        return 0.5 * ab.Cross(ac);
    }

    // Уточнил у наставника, в п7. задания нужно найти фигуру выше всего расположенную
    // на графике, а не наивысщую, соответственно метод Height должен возвращать максимальное
    // значение (с ечетом знака) y-координату
    [[nodiscard]] double Height() const { return std::max({a.y, b.y, c.y}); }

    [[nodiscard]] Point2D Center() const noexcept { return (a + b + c) / 3.0; }

    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        BoundingBox bb;
        bb.min_x = std::min({a.x, b.x, c.x});
        bb.min_y = std::min({a.y, b.y, c.y});
        bb.max_x = std::max({a.x, b.x, c.x});
        bb.max_y = std::max({a.y, b.y, c.y});
        return bb;
    }

    [[nodiscard]] std::array<Point2D, 3u> Vertices() const noexcept { return std::array{a, b, c}; }

    [[nodiscard]] Lines2D<4> Lines() const noexcept {
        Lines2D<4> res;
        res.x = {a.x, b.x, c.x, a.x};
        res.y = {a.y, b.y, c.y, a.y};
        return res;
    }
};

struct Rectangle {
    Point2D bottom_left;
    double width, height;

    /* ваш код здесь */
    [[nodiscard]] double Area() const noexcept { return height * width; }

    [[nodiscard]] double Height() const noexcept { return bottom_left.y + height; }

    [[nodiscard]] Point2D Center() const noexcept {
        return {bottom_left.x + width / 2.0, bottom_left.y + height / 2.0};
    }

    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        BoundingBox bb;
        bb.min_x = bottom_left.x;
        bb.min_y = bottom_left.y;
        bb.max_x = bottom_left.x + width;
        bb.max_y = bottom_left.y + height;
        return bb;
    }

    [[nodiscard]] std::array<Point2D, 4u> Vertices() const noexcept {
        std::array<Point2D, 4u> points;
        // Идем по часовой стрелке от левой нижней вершины
        points[0] = bottom_left;
        // левая верхняя вершина
        points[1] = {bottom_left.x, bottom_left.y + height};
        // правая верхняя вершина
        points[2] = {bottom_left.x + width, bottom_left.y + height};
        // правая нижняя вершина
        points[3] = {bottom_left.x + width, bottom_left.y};
        return points;
    }

    [[nodiscard]] Lines2D<5> Lines() const noexcept {
        Lines2D<5> lines;
        for (auto &&[index, vertex] : std::views::enumerate(Vertices())) {
            lines.x[index] = vertex.x;
            lines.y[index] = vertex.y;
        }
        lines.x.back() = lines.x.front();
        lines.y.back() = lines.y.front();
        return lines;
    }
};

struct RegularPolygon {
    Point2D center_p;
    double radius;
    int sides;

    constexpr RegularPolygon(Point2D center, double radius, int sides)
        : center_p(center), radius(radius), sides(sides) {}

    [[nodiscard]] double Height() const noexcept {
        // В случаи вписанной окружности h правельного многоугольника равна r - вписанной окружности
        return center_p.y + radius;  // по аналогии с Circle из прекода
    }

    [[nodiscard]] Point2D Center() const noexcept { return center_p; }

    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        BoundingBox bb;
        bb.max_x = std::numeric_limits<double>::lowest();
        bb.max_y = std::numeric_limits<double>::lowest();
        bb.min_x = std::numeric_limits<double>::max();
        bb.min_y = std::numeric_limits<double>::max();
        const auto vertices = Vertices();
        for (const auto &point : vertices) {
            bb.max_x = std::max(point.x, bb.max_x);
            bb.min_x = std::min(point.x, bb.min_x);
            bb.max_y = std::max(point.y, bb.max_y);
            bb.min_y = std::min(point.y, bb.min_y);
        }
        return bb;
    }

    [[nodiscard]] std::vector<Point2D> Vertices() const noexcept {
        std::vector<Point2D> points;
        points.reserve(sides);

        for (int i = 0; i < sides; ++i) {
            const double angle = 2 * std::numbers::pi * i / sides;
            points.emplace_back(center_p.x + radius * std::cos(angle), center_p.y + radius * std::sin(angle));
        }
        return points;
    }

    [[nodiscard]] Lines2DDyn Lines() const noexcept {
        Lines2DDyn lines;
        lines.Reserve(sides + 1);
        for (const auto &vertex : Vertices()) {
            lines.x.push_back(vertex.x);
            lines.y.push_back(vertex.y);
        }

        lines.x.push_back(lines.Front().x);
        lines.y.push_back(lines.Front().y);
        return lines;
    }
};

struct Circle {
    Point2D center_p;
    double radius;

    constexpr Circle(Point2D center, double radius) : center_p(center), radius(radius) {}

    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        return {center_p.x - radius, center_p.y - radius, center_p.x + radius, center_p.y + radius};
    }
    double Height() const noexcept { return center_p.y + radius; }
    Point2D Center() const noexcept { return center_p; }

    //
    // Должны быть сделана по аналогии с RegularPolygon::Vertices
    //
    std::vector<Point2D> Vertices(size_t N = 30) const {
        // сделать exception если N < 3 (минимальное число, для описания окружности)
        std::vector<Point2D> points;
        points.reserve(N);
        const double angleStep = (2 * std::numbers::pi) / N;
        for (size_t i = 0; i < N; ++i) {
            const double angele = i * angleStep;
            points.emplace_back(center_p.x + radius * cos(angele), center_p.y + radius * sin(angele));
        }
        return points;
    }

    Lines2DDyn Lines(size_t N = 100) const {
        Lines2DDyn lines;
        lines.Reserve(N + 1);
        for (const auto &vertex : Vertices(N)) {
            lines.x.push_back(vertex.x);
            lines.y.push_back(vertex.y);
        }
        const auto p = lines.Front();
        lines.x.push_back(p.x);
        lines.y.push_back(p.y);
        return lines;
    }
};

// произврльное хранилише для точек разных объектов, чтобы единообразно
// передать их в  алгоритм построения выпуклой оболочки или треангуляции
template <typename T>
concept Container = requires(T container) {
    // Проверяем, что можно получить итераторы
    { container.begin() } -> std::same_as<decltype(container.end())>;
    // Проверяем, что можно получить размер
    { container.size() } -> std::convertible_to<std::size_t>;
    // Проверяем, что можно получить элемент по итератору
    { *container.begin() };
};

class Polygon {
public:
    /* ваш код здесь */
    template <Container ContainerType>
    Polygon(const ContainerType &points) {
        points_.reserve(points_.size() + points.size());
        std::ranges::copy(points, std::back_inserter(points_));
        CalcBoudingBox();
    }

    void Clear() noexcept {
        points_.clear();
        bounding_box_.min_x = bounding_box_.min_y = bounding_box_.max_x = bounding_box_.max_y = 0.0;
    }

    template <Container ContainerType>
    void PushBack(const ContainerType &points) {
        points_.reserve(points_.size() + points.size());
        std::ranges::copy(points, std::back_inserter(points_));
        CalcBoudingBox();
    }

    [[nodiscard]] Point2D Center() const {
        const size_t N = points_.size();
        // формула центройда
        Point2D center{0.0, 0.0};
        double area = 0.0;
        for (size_t i = 0; i < points_.size(); i++) {
            const size_t j = (i + 1) % N;
            const auto &iPoint = points_[i];
            const auto &jPoint = points_[j];
            const double cross = iPoint.Cross(jPoint);
            area += cross;
            center = center + (iPoint + jPoint) * cross;
        }

        area *= 0.5;
        center = center / (6 * area);
        return center;
    }

    // Уточнил у наставника, в п7. задания нужно найти фигуру выше всего расположенную
    // на графике, а не наивысщую, соответственно метод Height должен возвращать максимальное
    // значение (с ечетом знака) y-координату
    [[nodiscard]] double Height() const {
        CheckPoints();
        return std::ranges::max(points_ | std::views::transform([](const Point2D &p) { return p.y; }));
    }

    [[nodiscard]] BoundingBox BoundBox() const noexcept { return bounding_box_; }

    [[nodiscard]] const std::vector<Point2D> &Vertices() const noexcept { return points_; }

    [[nodiscard]] Lines2DDyn Lines() const {
        Lines2DDyn lines;
        lines.Reserve(points_.size() + 1);
        for (const auto &vertex : Vertices()) {
            lines.x.push_back(vertex.x);
            lines.y.push_back(vertex.y);
        }
        const auto p = lines.Front();
        lines.x.push_back(p.x);
        lines.y.push_back(p.y);
        return lines;
    }

private:
    void CheckPoints() const {
        if (points_.empty())
            throw std::runtime_error("Error: There are no points in polygin");
    }

    void CalcBoudingBox() noexcept {
        bounding_box_.min_x = std::numeric_limits<double>::max();
        bounding_box_.min_y = std::numeric_limits<double>::max();
        bounding_box_.max_x = std::numeric_limits<double>::min();
        bounding_box_.max_y = std::numeric_limits<double>::min();
        for (const auto &point : points_) {
            bounding_box_.min_x = std::min(bounding_box_.min_x, point.x);
            bounding_box_.min_y = std::min(bounding_box_.min_y, point.y);
            bounding_box_.max_x = std::max(bounding_box_.max_x, point.x);
            bounding_box_.max_y = std::max(bounding_box_.max_y, point.y);
        }
    }

private:
    std::vector<Point2D> points_;
    BoundingBox bounding_box_;
};

using Shape = std::variant<Line, Triangle, Rectangle, RegularPolygon, Circle, Polygon>;

/*
 * В коде везде используется DummyClass. Ваша задача - выбрать наиболее подходящий тип для решения задачи
 */
struct DummyClass {
    DummyClass(std::vector<Shape>) {}
};

enum class GeometryError { Unsupported, NoIntersection, InvalidInput, DegenrateCase, InsufficientPoints };

inline std::string ConvertGeometryError(const GeometryError &geometryError) {
    auto comparator = [](const GeometryError &lhv, const GeometryError &rhv) {
        return std::to_underlying(lhv) < std::to_underlying(rhv);
    };
    const std::flat_map<GeometryError, std::string, decltype(comparator)> fromEnumToString = {
        {GeometryError::Unsupported, "Unsuported error"},
        {GeometryError::NoIntersection, "NoIntersection"},
        {GeometryError::InvalidInput, "InvalidInput"},
        {GeometryError::DegenrateCase, "DegenrateCase"},
        {GeometryError::InsufficientPoints, "InsufficientPoints"}};

    const auto it = fromEnumToString.find(geometryError);
    assert(it != fromEnumToString.end());
    return it->second;
}

template <typename T>
using GeometryResult = std::expected<T, GeometryError>;

}  // namespace geometry

template <>
struct std::formatter<geometry::Point2D> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Point2D &p, FormatContext &ctx) const {
        return format_to(ctx.out(), "({:.2f}, {:.2f})", p.x, p.y);
    }
};
template <>
struct std::formatter<std::vector<geometry::Point2D>> {
    bool use_new_line = false;

    constexpr auto parse(std::format_parse_context &ctx) {
        /* ваш код здесь */
        // Создаем string_view напрямую из строки
        const std::string_view substr(ctx);

        // приходит строка вида: new_line}
        // Находим позицию символа '}'
        const size_t brace_pos = substr.find('}');
        if (brace_pos != std::string::npos && substr.substr(0, brace_pos) == "new_line")
            use_new_line = true;

        return ctx.begin() + brace_pos;
    }

    template <typename FormatContext>
    auto format(const std::vector<geometry::Point2D> &v, FormatContext &ctx) const {
        for (const auto &p : v) {
            use_new_line ? std::format_to(ctx.out(), "{}\n\t", p) : std::format_to(ctx.out(), "{} ", p);
        }

        /* ваш код здесь */
        return ctx.out();
    }
};

template <>
struct std::formatter<geometry::Line> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Line &l, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Line({}, {})", l.start, l.end);
    }
};

template <>
struct std::formatter<geometry::Circle> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Circle &c, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Circle(center={}, r={:.2f})", c.center_p, c.radius);
    }
};

template <>
struct std::formatter<geometry::Rectangle> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Rectangle &r, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Rectangle(bottom_left={}, w={:.2f}, h={:.2f})", r.bottom_left, r.width,
                              r.height);
    }
};

template <>
struct std::formatter<geometry::RegularPolygon> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::RegularPolygon &p, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "RegularPolygon(center={}, r={:.2f}, sides={})", p.center_p, p.radius,
                              p.sides);
    }
};
template <>
struct std::formatter<geometry::Triangle> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Triangle &t, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Triangle({}, {}, {})", t.a, t.b, t.c);
    }
};
template <>
struct std::formatter<geometry::Polygon> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Polygon &poly, FormatContext &ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "Polygon[{} points]: [", poly.Vertices().size());

        for (const auto &p : poly.Vertices()) {
            out = std::format_to(out, "{} ", p);
        }

        return std::format_to(out, "]");
    }
};
