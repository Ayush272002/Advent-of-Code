#include <algorithm>
#include <concepts>
#include <functional>
#include <iostream>
#include <map>
#include <print>
#include <ranges>
#include <set>
#include <string>
#include <vector>

#include "AOC.hpp"

template <typename T>
concept Integral = std::integral<T>;

template <typename T>
concept Coordinate = requires(T t) {
    { t.row } -> std::convertible_to<int>;
    { t.col } -> std::convertible_to<int>;
};

struct Position {
    int row;
    int col;

    constexpr auto operator<=>(const Position&) const = default;

    [[nodiscard]] constexpr auto move_down() const noexcept -> Position {
        return {row + 1, col};
    }

    [[nodiscard]] constexpr auto move_left() const noexcept -> Position {
        return {row + 1, col - 1};
    }

    [[nodiscard]] constexpr auto move_right() const noexcept -> Position {
        return {row + 1, col + 1};
    }
};

enum class CellType : char { EMPTY = '.', SPLITTER = '^', START = 'S' };

constexpr auto to_cell_type(char c) -> CellType {
    switch (c) {
        case '^':
            return CellType::SPLITTER;
        case 'S':
            return CellType::START;
        default:
            return CellType::EMPTY;
    }
}

template <typename T = std::string>
class Grid {
public:
    explicit constexpr Grid(const std::vector<T>& data_)
        : data(data_),
          rows(static_cast<int>(data_.size())),
          cols(data_.empty() ? 0 : static_cast<int>(data_[0].size())) {}

    [[nodiscard]] constexpr auto getRows() const noexcept -> int {
        return rows;
    }
    [[nodiscard]] constexpr auto getCols() const noexcept -> int {
        return cols;
    }

    [[nodiscard]] constexpr auto isValid(Position pos) const noexcept -> bool {
        return pos.row >= 0 && pos.col >= 0 && pos.row < rows && pos.col < cols;
    }

    [[nodiscard]] constexpr auto at(Position pos) const -> char {
        return data[pos.row][pos.col];
    }

    [[nodiscard]] constexpr auto cellType(Position pos) const -> CellType {
        return to_cell_type(at(pos));
    }

    [[nodiscard]] auto findStart() const -> Position {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (data[r][c] == 'S') {
                    return {r, c};
                }
            }
        }
        return {0, 0};
    }

    [[nodiscard]] auto positions() const {
        return std::views::iota(0, rows) | std::views::transform([this](int r) {
                   return std::views::iota(0, cols) |
                          std::views::transform(
                              [r](int c) { return Position{r, c}; });
               }) |
               std::views::join;
    }

private:
    const std::vector<T>& data;
    int rows, cols;
};

template <Integral T = long long>
class BeamSimulator {
public:
    explicit constexpr BeamSimulator(const Grid<>& grid_) : grid(grid_) {}

    [[nodiscard]] auto count_splits(const Position start) const -> T {
        std::set<int> beam_positions{start.col};
        T total_splits = 0;

        for (int row : std::views::iota(0, grid.getRows())) {
            auto [next_pos, splits] = process_row(beam_positions, row);
            beam_positions = std::move(next_pos);
            total_splits += splits;
        }

        return total_splits;
    }

private:
    const Grid<>& grid;

    [[nodiscard]] auto process_row(const std::set<int>& current_positions,
                                   const int row) const
        -> std::pair<std::set<int>, T> {
        std::set<int> next_positions;
        T splits = 0;

        for (int col : current_positions) {
            Position pos{row, col};

            if (!grid.isValid(pos)) continue;

            switch (grid.cellType(pos)) {
                case CellType::SPLITTER:
                    ++splits;
                    if (col - 1 >= 0) next_positions.insert(col - 1);
                    if (col + 1 < grid.getCols())
                        next_positions.insert(col + 1);
                    break;

                case CellType::EMPTY:
                case CellType::START:
                    next_positions.insert(col);
                    break;
            }
        }

        return {next_positions, splits};
    }
};

template <Integral T = long long>
class QuantumTimelineCounter {
public:
    explicit constexpr QuantumTimelineCounter(const Grid<>& grid_)
        : grid(grid_) {}

    [[nodiscard]] constexpr auto count_timelines(const Position start) const
        -> T {
        memo.clear();
        return count_impl(start);
    }

private:
    const Grid<>& grid;
    mutable std::map<Position, T> memo;

    [[nodiscard]] auto count_impl(Position pos) const -> T {
        if (!grid.isValid(pos)) return T{1};

        if (auto it = memo.find(pos); it != memo.end()) return it->second;

        T result = 0;

        switch (grid.cellType(pos)) {
            case CellType::SPLITTER: {
                // Quantum split: particle takes both paths
                const Position left = pos.move_left();
                const Position right = pos.move_right();

                if (grid.isValid({pos.row + 1, left.col}))
                    result += count_impl(left);
                if (grid.isValid({pos.row + 1, right.col}))
                    result += count_impl(right);
                break;
            }

            case CellType::EMPTY:
            case CellType::START:
                result = count_impl(pos.move_down());
                break;
        }

        memo[pos] = result;
        return result;
    }
};

template <typename Derived, Integral T = long long>
class SolutionStrategy {
public:
    [[nodiscard]] auto solve() const -> T {
        Position start = grid.findStart();
        return static_cast<const Derived*>(this)->solve_impl(start);
    }

protected:
    const Grid<> grid;

    explicit constexpr SolutionStrategy(const std::vector<std::string>& input)
        : grid(input) {}
};

template <Integral T = long long>
class Part1Solution : public SolutionStrategy<Part1Solution<T>, T> {
public:
    explicit constexpr Part1Solution(const std::vector<std::string>& input)
        : SolutionStrategy<Part1Solution<T>, T>(input) {}

private:
    friend SolutionStrategy<Part1Solution<T>, T>;

    [[nodiscard]] auto solve_impl(Position start) const -> T {
        BeamSimulator<T> simulator(this->grid);
        return simulator.count_splits(start);
    }
};

template <Integral T = long long>
class Part2Solution : public SolutionStrategy<Part2Solution<T>, T> {
public:
    explicit constexpr Part2Solution(const std::vector<std::string>& input)
        : SolutionStrategy<Part2Solution<T>, T>(input) {}

private:
    friend SolutionStrategy<Part2Solution<T>, T>;

    [[nodiscard]] auto solve_impl(Position start) const -> T {
        QuantumTimelineCounter<T> counter(this->grid);
        return counter.count_timelines(start);
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
        std::vector<std::string> input = AOC::fetchAOCInputVector(2025, 7);

        std::string part1_ans = std::to_string(part1(input));
        std::string part2_ans = std::to_string(part2(input));

        std::println("Part 1 ans : {}", part1_ans);
        std::println("Part 2 ans : {}", part2_ans);

        AOC::submitAnswer(2025, 7, 1, part1_ans);
        AOC::submitAnswer(2025, 7, 2, part2_ans);
    } catch (const std::exception& e) {
        std::cerr << "Error fetching input: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
