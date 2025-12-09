#include <algorithm>
#include <concepts>
#include <iostream>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

#include "AOC.hpp"

template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <typename T>
concept Point2D = requires(T t) {
    { t.x } -> std::convertible_to<long long>;
    { t.y } -> std::convertible_to<long long>;
};

template <typename T>
concept Rectangle = requires(T t) {
    { t.area() } -> std::convertible_to<long long>;
    { t.width() } -> std::convertible_to<long long>;
    { t.height() } -> std::convertible_to<long long>;
};

template <Numeric T = long long>
struct Point {
    T x, y;

    constexpr auto operator<=>(const Point&) const = default;

    [[nodiscard]] constexpr auto manhattanDistance(
        const Point& other) const noexcept -> T {
        return std::abs(x - other.x) + std::abs(y - other.y);
    }

    [[nodiscard]] constexpr auto euclideanDistanceSquared(
        const Point& other) const noexcept -> T {
        T dx = x - other.x;
        T dy = y - other.y;
        return dx * dx + dy * dy;
    }
};

template <Numeric T = long long>
class Rect {
public:
    constexpr Rect(Point<T> p1, Point<T> p2) noexcept
        : topLeft_{std::min(p1.x, p2.x), std::min(p1.y, p2.y)},
          bottomRight_{std::max(p1.x, p2.x), std::max(p1.y, p2.y)} {}

    [[nodiscard]] constexpr auto width() const noexcept -> T {
        return std::abs(bottomRight_.x - topLeft_.x) + 1;
    }

    [[nodiscard]] constexpr auto height() const noexcept -> T {
        return std::abs(bottomRight_.y - topLeft_.y) + 1;
    }

    [[nodiscard]] constexpr auto area() const noexcept -> T {
        return width() * height();
    }

    [[nodiscard]] constexpr auto isDegenerate() const noexcept -> bool {
        return width() == T{1} || height() == T{1};
    }

    [[nodiscard]] constexpr auto topLeft() const noexcept -> Point<T> {
        return topLeft_;
    }

    [[nodiscard]] constexpr auto bottomRight() const noexcept -> Point<T> {
        return bottomRight_;
    }

private:
    Point<T> topLeft_;
    Point<T> bottomRight_;
};

template <Numeric T = long long>
class PointParser {
public:
    [[nodiscard]] static auto parse(const std::vector<std::string>& input)
        -> std::vector<Point<T>> {
        return input | std::views::filter([](const auto& line) {
                   return !line.empty();
               }) |
               std::views::transform(
                   [](const auto& line) { return parseLine(line); }) |
               std::ranges::to<std::vector>();
    }

private:
    [[nodiscard]] static auto parseLine(const std::string& line) -> Point<T> {
        T x, y;

        if (line.find(',') != std::string::npos) {
            std::size_t commaPos = line.find(',');
            x = static_cast<T>(std::stoll(line.substr(0, commaPos)));
            y = static_cast<T>(std::stoll(line.substr(commaPos + 1)));
        } else {
            std::stringstream ss(line);
            ss >> x >> y;
        }

        return {x, y};
    }
};

template <Numeric T = long long>
class RectangleFinder {
public:
    explicit RectangleFinder(const std::vector<Point<T>>& points)
        : points_(points) {}

    [[nodiscard]] auto findLargestRectangle() const -> T {
        if (points_.size() < 2) {
            return T{0};
        }
        return findMaxAreaBruteForce();
    }

    [[nodiscard]] auto findLargestRectangleInsidePolygon() const -> T {
        if (points_.size() < 3) {
            return T{0};
        }
        return findMaxAreaWithPolygonConstraint();
    }

private:
    const std::vector<Point<T>>& points_;

    [[nodiscard]] auto findMaxAreaBruteForce() const -> T {
        const std::size_t n = points_.size();
        T maxArea = T{0};

        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = i + 1; j < n; ++j) {
                Rect<T> rect(points_[i], points_[j]);
                maxArea = std::max(maxArea, rect.area());
            }
        }

        return maxArea;
    }

    [[nodiscard]] auto isPointInPolygon(Point<T> p) const -> bool {
        const std::size_t n = points_.size();
        bool inside = false;

        for (std::size_t i = 0, j = n - 1; i < n; j = i++) {
            const auto& pi = points_[i];
            const auto& pj = points_[j];

            if (isPointOnSegment(p, pj, pi)) {
                return true;
            }

            if (((pi.y > p.y) != (pj.y > p.y)) &&
                (p.x < (pj.x - pi.x) * (p.y - pi.y) / (pj.y - pi.y) + pi.x)) {
                inside = !inside;
            }
        }

        return inside;
    }

    [[nodiscard]] static auto isPointOnSegment(Point<T> p, Point<T> a,
                                               Point<T> b) -> bool {
        T crossProduct = (p.y - a.y) * (b.x - a.x) - (p.x - a.x) * (b.y - a.y);
        if (crossProduct != 0) return false;

        if (p.x < std::min(a.x, b.x) || p.x > std::max(a.x, b.x)) return false;
        if (p.y < std::min(a.y, b.y) || p.y > std::max(a.y, b.y)) return false;

        return true;
    }

    [[nodiscard]] auto isRectangleValid(const Rect<T>& rect) const -> bool {
        Point<T> topLeft = rect.topLeft();
        Point<T> bottomRight = rect.bottomRight();
        Point<T> topRight = {bottomRight.x, topLeft.y};
        Point<T> bottomLeft = {topLeft.x, bottomRight.y};

        if (!isPointInPolygon(topLeft) || !isPointInPolygon(topRight) ||
            !isPointInPolygon(bottomLeft) || !isPointInPolygon(bottomRight)) {
            return false;
        }

        T width = rect.width();
        T height = rect.height();

        T stepX = std::max(T{1}, width / 100);
        T stepY = std::max(T{1}, height / 100);

        for (T x = topLeft.x; x <= bottomRight.x; x += stepX) {
            if (!isPointInPolygon({x, topLeft.y}) ||
                !isPointInPolygon({x, bottomRight.y})) {
                return false;
            }
        }

        for (T y = topLeft.y; y <= bottomRight.y; y += stepY) {
            if (!isPointInPolygon({topLeft.x, y}) ||
                !isPointInPolygon({bottomRight.x, y})) {
                return false;
            }
        }

        Point<T> center = {(topLeft.x + bottomRight.x) / 2,
                           (topLeft.y + bottomRight.y) / 2};
        if (!isPointInPolygon(center)) {
            return false;
        }

        return true;
    }

    [[nodiscard]] auto findMaxAreaWithPolygonConstraint() const -> T {
        const std::size_t n = points_.size();
        T maxArea = T{0};

        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = i + 1; j < n; ++j) {
                Rect<T> rect(points_[i], points_[j]);

                if (isRectangleValid(rect)) {
                    maxArea = std::max(maxArea, rect.area());
                }
            }
        }

        return maxArea;
    }
};

template <typename Derived, Numeric T = long long>
class SolutionStrategy {
public:
    explicit SolutionStrategy(const std::vector<std::string>& input)
        : points_(PointParser<T>::parse(input)) {}

    [[nodiscard]] auto solve() const -> T {
        return static_cast<const Derived*>(this)->solveImpl();
    }

    [[nodiscard]] auto getPoints() const -> const std::vector<Point<T>>& {
        return points_;
    }

protected:
    std::vector<Point<T>> points_;
};

template <Numeric T = long long>
class Part1Solution : public SolutionStrategy<Part1Solution<T>, T> {
public:
    using SolutionStrategy<Part1Solution<T>, T>::SolutionStrategy;

    [[nodiscard]] auto solveImpl() const -> T {
        RectangleFinder<T> finder(this->points_);
        return finder.findLargestRectangle();
    }
};

template <Numeric T = long long>
class Part2Solution : public SolutionStrategy<Part2Solution<T>, T> {
public:
    using SolutionStrategy<Part2Solution<T>, T>::SolutionStrategy;

    [[nodiscard]] auto solveImpl() const -> T {
        RectangleFinder<T> finder(this->points_);
        return finder.findLargestRectangleInsidePolygon();
    }
};

[[nodiscard]] inline auto part1(const std::vector<std::string>& input)
    -> long long {
    return Part1Solution<>(input).solve();
}

[[nodiscard]] inline auto part2(const std::vector<std::string>& input)
    -> long long {
    return Part2Solution<>(input).solve();
}

int main() {
    try {
        std::vector<std::string> input = AOC::fetchAOCInputVector(2025, 9);

        std::string part1_ans = std::to_string(part1(input));
        std::string part2_ans = std::to_string(part2(input));

        std::println("Part 1 ans : {}", part1_ans);
        std::println("Part 2 ans : {}", part2_ans);

        AOC::submitAnswer(2025, 9, 1, part1_ans);
        AOC::submitAnswer(2025, 9, 2, part2_ans);
    } catch (const std::exception& e) {
        std::cerr << "Error fetching input: " << e.what() << "\n";
        return 1;
    }

    return 0;
}