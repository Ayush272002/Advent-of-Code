#include <algorithm>
#include <concepts>
#include <functional>
#include <iostream>
#include <numeric>
#include <optional>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include "AOC.hpp"

template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <typename T>
concept StringLike = std::convertible_to<T, std::string_view>;

struct ColumnIndex {
    std::size_t value;

    constexpr auto operator<=>(const ColumnIndex&) const = default;
};

struct RowIndex {
    std::size_t value;

    constexpr auto operator<=>(const RowIndex&) const = default;
};

struct ColumnRange {
    ColumnIndex start;
    ColumnIndex end;

    [[nodiscard]] constexpr auto width() const noexcept -> std::size_t {
        return end.value - start.value + 1;
    }

    [[nodiscard]] constexpr auto contains(const ColumnIndex col) const noexcept
        -> bool {
        return col.value >= start.value && col.value <= end.value;
    }
};

enum class Operation : char { ADD = '+', MULTIPLY = '*' };

constexpr auto to_operation(const char c) -> std::optional<Operation> {
    switch (c) {
        case '+':
            return Operation::ADD;
        case '*':
            return Operation::MULTIPLY;
        default:
            return std::nullopt;
    }
}

template <Numeric T = long long>
struct Problem {
    std::vector<T> numbers;
    Operation operation;

    [[nodiscard]] constexpr auto evaluate() const noexcept -> T {
        if (numbers.empty()) return T{0};

        return operation == Operation::ADD
                   ? std::reduce(numbers.begin(), numbers.end(), T{0},
                                 std::plus{})
                   : std::reduce(numbers.begin(), numbers.end(), T{1},
                                 std::multiplies{});
    }
};

template <StringLike S = std::string>
class Grid {
public:
    explicit constexpr Grid(const std::vector<S>& data)
        : data_(data),
          height_(data.size()),
          width_(data.empty() ? 0 : data[0].size()) {}

    [[nodiscard]] constexpr auto height() const noexcept -> std::size_t {
        return height_;
    }

    [[nodiscard]] constexpr auto width() const noexcept -> std::size_t {
        return width_;
    }

    [[nodiscard]] constexpr auto last_row() const noexcept -> RowIndex {
        return {height_ - 1};
    }

    [[nodiscard]] constexpr auto at(RowIndex row, ColumnIndex col) const
        -> char {
        return data_[row.value][col.value];
    }

    [[nodiscard]] auto is_column_empty(ColumnIndex col) const -> bool {
        return std::ranges::all_of(
            std::views::iota(0uz, height_),
            [this, col](auto r) { return at({r}, col) == ' '; });
    }

    [[nodiscard]] auto extract_column(ColumnIndex col, RowIndex max_row) const
        -> std::string {
        return std::views::iota(0uz, max_row.value) |
               std::views::transform(
                   [this, col](auto r) { return at({r}, col); }) |
               std::views::filter([](const char c) { return c != ' '; }) |
               std::ranges::to<std::string>();
    }

    [[nodiscard]] auto extract_row_range(RowIndex row,
                                         const ColumnRange range) const
        -> std::string_view {
        return std::string_view(data_[row.value])
            .substr(range.start.value, range.width());
    }

private:
    const std::vector<S>& data_;
    std::size_t height_;
    std::size_t width_;
};

class BlockFinder {
public:
    template <typename GridType>
    [[nodiscard]] static auto find_blocks(const GridType& grid)
        -> std::vector<ColumnRange> {
        std::vector<ColumnRange> blocks;
        ColumnIndex col{};

        while (col.value < grid.width()) {
            auto start = find_next_non_empty_column(grid, col);
            if (!start) break;

            auto end = find_next_empty_column(grid, *start);
            blocks.emplace_back(*start, ColumnIndex{end.value - 1});
            col = end;
        }

        return blocks;
    }

private:
    template <typename GridType>
    static auto find_next_non_empty_column(const GridType& grid,
                                           ColumnIndex start)
        -> std::optional<ColumnIndex> {
        for (auto col = start.value; col < grid.width(); ++col) {
            if (!grid.is_column_empty({col})) {
                return ColumnIndex{col};
            }
        }
        return std::nullopt;
    }

    template <typename GridType>
    static auto find_next_empty_column(const GridType& grid, ColumnIndex start)
        -> ColumnIndex {
        for (auto col = start.value; col < grid.width(); ++col) {
            if (grid.is_column_empty({col})) {
                return {col};
            }
        }
        return {grid.width()};
    }
};

template <Numeric T = long long>
class ParserStrategy {
public:
    virtual ~ParserStrategy() = default;

    virtual auto parse(const Grid<>& grid, const ColumnRange& range)
        -> Problem<T> = 0;
};

template <Numeric T = long long>
class Part1Parser final : public ParserStrategy<T> {
public:
    auto parse(const Grid<>& grid, const ColumnRange& range)
        -> Problem<T> override {
        return Problem<T>{.numbers = extract_numbers(grid, range),
                          .operation = extract_operation(grid, range)};
    }

private:
    [[nodiscard]] static auto extract_operation(const Grid<>& grid,
                                                const ColumnRange& range)
        -> Operation {
        const auto last_row = grid.last_row();

        for (auto c = range.start.value; c < range.end.value; ++c) {
            const char ch = grid.at(last_row, {c});
            if (const auto op = to_operation(ch)) return *op;
        }

        return Operation::ADD;
    }

    [[nodiscard]] static auto extract_numbers(const Grid<>& grid,
                                              const ColumnRange& range)
        -> std::vector<T> {
        auto rows = std::views::iota(0uz, grid.last_row().value);

        return rows | std::views::transform([&](auto r) {
                   auto row_text = grid.extract_row_range({r}, range);
                   auto trimmed =
                       row_text |
                       std::views::drop_while([](char c) { return c == ' '; }) |
                       std::views::reverse |
                       std::views::drop_while([](char c) { return c == ' '; }) |
                       std::views::reverse | std::ranges::to<std::string>();

                   return trimmed.empty() ? std::optional<T>{}
                                          : std::optional<T>{static_cast<T>(
                                                std::stoll(trimmed))};
               }) |
               std::views::filter(
                   [](const auto& opt) { return opt.has_value(); }) |
               std::views::transform([](const auto& opt) { return *opt; }) |
               std::ranges::to<std::vector>();
    }
};

template <Numeric T = long long>
class Part2Parser final : public ParserStrategy<T> {
public:
    auto parse(const Grid<>& grid, const ColumnRange& range)
        -> Problem<T> override {
        return Problem<T>{.numbers = extract_numbers(grid, range),
                          .operation = extract_operation(grid, range)};
    }

private:
    [[nodiscard]] static auto extract_operation(const Grid<>& grid,
                                                const ColumnRange& range)
        -> Operation {
        const auto last_row = grid.last_row();

        for (auto c = range.start.value; c <= range.end.value; ++c) {
            const char ch = grid.at(last_row, {c});
            if (const auto op = to_operation(ch)) {
                return *op;
            }
        }
        return Operation::ADD;
    }

    [[nodiscard]] static auto extract_numbers(const Grid<>& grid,
                                              const ColumnRange& range)
        -> std::vector<T> {
        // Right-to-left
        auto columns =
            std::views::iota(range.start.value, range.end.value + 1) |
            std::views::reverse;

        return columns | std::views::transform([&](auto c) {
                   return grid.extract_column({c}, grid.last_row());
               }) |
               std::views::filter([](const auto& s) { return !s.empty(); }) |
               std::views::transform([](const auto& s) {
                   return static_cast<T>(std::stoll(s));
               }) |
               std::ranges::to<std::vector>();
    }
};

template <typename Derived, Numeric T = long long>
class WorksheetSolver {
public:
    [[nodiscard]] auto solve(const std::vector<std::string>& input) -> T {
        auto problems = parse_problems(input);

        return std::ranges::fold_left(
            problems | std::views::transform(&Problem<T>::evaluate), T{0},
            std::plus{});
    }

protected:
    [[nodiscard]] auto parse_problems(const std::vector<std::string>& input)
        -> std::vector<Problem<T> > {
        Grid grid(input);
        auto blocks = BlockFinder::find_blocks(grid);
        auto& parser = static_cast<Derived*>(this)->get_parser();

        return blocks | std::views::transform([&](const auto& range) {
                   return parser.parse(grid, range);
               }) |
               std::ranges::to<std::vector>();
    }
};

template <Numeric T = long long>
class Part1Solver : public WorksheetSolver<Part1Solver<T>, T> {
public:
    [[nodiscard]] auto get_parser() -> Part1Parser<T>& { return parser_; }

private:
    Part1Parser<T> parser_;
};

template <Numeric T = long long>
class Part2Solver : public WorksheetSolver<Part2Solver<T>, T> {
public:
    [[nodiscard]] auto get_parser() -> Part2Parser<T>& { return parser_; }

private:
    Part2Parser<T> parser_;
};

[[nodiscard]] inline auto part1(const std::vector<std::string>& input)
    -> long long {
    return Part1Solver{}.solve(input);
}

[[nodiscard]] inline auto part2(const std::vector<std::string>& input)
    -> long long {
    return Part2Solver{}.solve(input);
}

int main() {
    try {
        const auto input = AOC::fetchAOCInputVector(2025, 6);
        std::println("Input lines: {}", input.size());

        const auto part1_ans = std::to_string(part1(input));
        const auto part2_ans = std::to_string(part2(input));

        std::println("Part 1: {}", part1_ans);
        std::println("Part 2: {}", part2_ans);

        AOC::submitAnswer(2025, 6, 1, part1_ans);
        AOC::submitAnswer(2025, 6, 2, part2_ans);
    } catch (const std::exception& e) {
        std::cerr << std::format("Error: {}\n", e.what());
        return 1;
    }

    return 0;
}
