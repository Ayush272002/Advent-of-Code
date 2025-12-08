#include <algorithm>
#include <concepts>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <print>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "AOC.hpp"

template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <typename T>
concept Point3D = requires(T t) {
    { t.x } -> std::convertible_to<long long>;
    { t.y } -> std::convertible_to<long long>;
    { t.z } -> std::convertible_to<long long>;
};

struct Point {
    long long x, y, z;

    constexpr auto operator<=>(const Point&) const = default;

    [[nodiscard]] constexpr auto distanceSquared(
        const Point& other) const noexcept -> long long {
        const long long dx = x - other.x;
        const long long dy = y - other.y;
        const long long dz = z - other.z;
        return dx * dx + dy * dy + dz * dz;
    }
};

struct Edge {
    int u, v;
    long long distanceSquared;

    constexpr auto operator<=>(const Edge&) const = default;
};

template <Numeric T = int>
class DisjointSetUnion {
public:
    explicit DisjointSetUnion(T n) : parent_(n), size_(n, 1) {
        std::iota(parent_.begin(), parent_.end(), T{0});
    }

    [[nodiscard]] auto find(T x) -> T {
        if (parent_[x] != x) {
            parent_[x] = find(parent_[x]);
        }
        return parent_[x];
    }

    auto unite(T a, T b) -> bool {
        a = find(a);
        b = find(b);

        if (a == b) return false;

        if (size_[a] < size_[b]) {
            std::swap(a, b);
        }

        parent_[b] = a;
        size_[a] += size_[b];
        return true;
    }

    [[nodiscard]] auto getSize(T x) -> T { return size_[find(x)]; }

    [[nodiscard]] auto isConnected(T a, T b) -> bool {
        return find(a) == find(b);
    }

    [[nodiscard]] auto countComponents() const -> std::size_t {
        std::set<T> roots;
        for (std::size_t i = 0; i < parent_.size(); ++i) {
            roots.insert(parent_[i]);
        }
        return roots.size();
    }

private:
    std::vector<T> parent_;
    std::vector<T> size_;
};

class InputParser {
public:
    [[nodiscard]] static auto parse(const std::vector<std::string>& input)
        -> std::pair<std::vector<Point>, std::vector<Edge>> {
        auto points = parsePoints(input);
        auto edges = generateEdges(points);
        return {std::move(points), std::move(edges)};
    }

private:
    [[nodiscard]] static auto parsePoints(const std::vector<std::string>& input)
        -> std::vector<Point> {
        return input |
               std::views::filter([](const auto& s) { return !s.empty(); }) |
               std::views::transform(
                   [](const auto& s) { return parsePoint(s); }) |
               std::ranges::to<std::vector>();
    }

    [[nodiscard]] static auto parsePoint(const std::string& line) -> Point {
        long long x, y, z;

        if (line.find(',') != std::string::npos) {
            // CSV format: x,y,z
            std::size_t pos1 = line.find(',');
            std::size_t pos2 = line.find(',', pos1 + 1);

            x = std::stoll(line.substr(0, pos1));
            y = std::stoll(line.substr(pos1 + 1, pos2 - pos1 - 1));
            z = std::stoll(line.substr(pos2 + 1));
        } else {
            // Space-separated format: x y z
            std::stringstream ss(line);
            ss >> x >> y >> z;
        }

        return {x, y, z};
    }

    [[nodiscard]] static auto generateEdges(const std::vector<Point>& points)
        -> std::vector<Edge> {
        const int n = static_cast<int>(points.size());
        std::vector<Edge> edges;
        edges.reserve(static_cast<std::size_t>(n) * (n - 1) / 2);

        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                const long long dist2 = points[i].distanceSquared(points[j]);
                edges.push_back({i, j, dist2});
            }
        }

        std::ranges::sort(edges, {}, &Edge::distanceSquared);
        return edges;
    }
};

class ConstellationAnalyzer {
public:
    explicit ConstellationAnalyzer(const std::vector<Point>& points,
                                   const std::vector<Edge>& edges)
        : points_(points), edges_(edges) {}

    [[nodiscard]] auto findTopThreeComponentsProduct(int maxEdges) const
        -> long long {
        const int n = static_cast<int>(points_.size());
        if (n == 0) return 0;

        DisjointSetUnion dsu(n);

        int processed = 0;
        for (const auto& edge : edges_) {
            if (processed >= maxEdges) break;
            dsu.unite(edge.u, edge.v);
            ++processed;
        }

        const auto componentSizes = getComponentSizes(dsu, n);

        if (componentSizes.size() < 3) return 0;

        return componentSizes[0] * componentSizes[1] * componentSizes[2];
    }

    [[nodiscard]] auto findLastConnectingEdgeProduct() const -> long long {
        const int n = static_cast<int>(points_.size());
        if (n == 0) return 0;

        DisjointSetUnion dsu(n);
        int lastU = -1, lastV = -1;

        for (const auto& edge : edges_) {
            if (dsu.unite(edge.u, edge.v)) {
                lastU = edge.u;
                lastV = edge.v;

                if (isFullyConnected(dsu, n)) {
                    break;
                }
            }
        }

        if (lastU == -1 || lastV == -1) return 0;

        return points_[lastU].x * points_[lastV].x;
    }

private:
    const std::vector<Point>& points_;
    const std::vector<Edge>& edges_;

    [[nodiscard]] static auto getComponentSizes(
        const DisjointSetUnion<int>& dsu, int n) -> std::vector<long long> {
        std::map<int, long long> componentMap;

        for (int i = 0; i < n; ++i) {
            componentMap[const_cast<DisjointSetUnion<int>&>(dsu).find(i)]++;
        }

        auto sizes =
            componentMap | std::views::values | std::ranges::to<std::vector>();

        std::ranges::sort(sizes, std::greater<>{});
        return sizes;
    }

    [[nodiscard]] static auto isFullyConnected(DisjointSetUnion<int>& dsu,
                                               int n) -> bool {
        int root = dsu.find(0);
        return std::ranges::all_of(std::views::iota(1, n), [&dsu, root](int i) {
            return dsu.find(i) == root;
        });
    }
};

template <typename Derived>
class SolutionStrategy {
public:
    explicit SolutionStrategy(const std::vector<std::string>& input)
        : points_(std::get<0>(InputParser::parse(input))),
          edges_(std::get<1>(InputParser::parse(input))) {}

    [[nodiscard]] auto solve() const -> long long {
        return static_cast<const Derived*>(this)->solveImpl();
    }

protected:
    std::vector<Point> points_;
    std::vector<Edge> edges_;
};

class Part1Solution : public SolutionStrategy<Part1Solution> {
public:
    using SolutionStrategy<Part1Solution>::SolutionStrategy;

    [[nodiscard]] auto solveImpl() const -> long long {
        ConstellationAnalyzer analyzer(points_, edges_);
        return analyzer.findTopThreeComponentsProduct(1000);
    }
};

class Part2Solution : public SolutionStrategy<Part2Solution> {
public:
    using SolutionStrategy<Part2Solution>::SolutionStrategy;

    [[nodiscard]] auto solveImpl() const -> long long {
        ConstellationAnalyzer analyzer(points_, edges_);
        return analyzer.findLastConnectingEdgeProduct();
    }
};

[[nodiscard]] inline auto part1(const std::vector<std::string>& input)
    -> long long {
    return Part1Solution(input).solve();
}

[[nodiscard]] inline auto part2(const std::vector<std::string>& input)
    -> long long {
    return Part2Solution(input).solve();
}

int main() {
    try {
        const std::vector<std::string> input =
            AOC::fetchAOCInputVector(2025, 8);

        std::string part1_ans = std::to_string(part1(input));
        std::string part2_ans = std::to_string(part2(input));

        std::println("Part 1 ans : {}", part1_ans);
        std::println("Part 2 ans : {}", part2_ans);

        AOC::submitAnswer(2025, 8, 1, part1_ans);
        AOC::submitAnswer(2025, 8, 2, part2_ans);
    } catch (const std::exception& e) {
        std::cerr << "Error fetching input: " << e.what() << "\n";
        return 1;
    }

    return 0;
}