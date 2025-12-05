#include <algorithm>
#include <iostream>
#include <print>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "AOC.hpp"

using Range = std::pair<long long, long long>;

std::pair<std::vector<Range>, std::vector<long long>> parseInput(
    std::string_view input) {
    std::istringstream ss((input.data()));
    std::string line;

    std::vector<Range> ranges;
    std::vector<long long> ids;

    while (std::getline(ss, line) && !line.empty()) {
        long long a, b;
        char dash;
        std::istringstream r(line);
        r >> a >> dash >> b;
        ranges.emplace_back(a, b);
    }

    while (std::getline(ss, line)) {
        if (!line.empty()) ids.emplace_back(std::stoll(line));
    }

    return {ranges, ids};
}

static std::vector<Range> mergeRanges(std::vector<Range>& ranges) {
    std::ranges::sort(ranges);

    std::vector<Range> merged;
    for (auto& r : ranges) {
        if (merged.empty() || r.first > merged.back().second + 1)
            merged.push_back(r);
        else
            merged.back().second = std::max(merged.back().second, r.second);
    }

    return merged;
}

static bool isFresh(long long id, const std::vector<Range>& merged) {
    int lo = 0, hi = static_cast<int>(merged.size()) - 1;

    while (lo <= hi) {
        const int mid = lo + (hi - lo) / 2;
        auto [l, r] = merged[mid];
        if (id < l)
            hi = mid - 1;
        else if (id > r)
            lo = mid + 1;
        else
            return true;
    }

    return false;
}

int part1(const std::pair<std::vector<Range>, std::vector<long long>>& input) {
    auto [ranges, ids] = input;

    const auto merged = mergeRanges(ranges);

    int count = 0;
    for (const auto& id : ids) {
        if (isFresh(id, merged)) ++count;
    }

    return count;
}

long long part2(
    const std::pair<std::vector<Range>, std::vector<long long>>& input) {
    auto [ranges, _] = input;
    auto merged = mergeRanges(ranges);
    long long total = 0;

    for (auto& [l, r] : merged) {
        total += (r - l + 1);
    }

    return total;
}

int main() {
    try {
        const std::string input = AOC::fetchRawInput(2025, 5);
        const auto parsedInput = parseInput(input);

        std::string part1_ans = std::to_string(part1(parsedInput));
        std::string part2_ans = std::to_string(part2(parsedInput));

        std::println("Part 1 ans : {}", part1_ans);
        std::println("Part 2 ans : {}", part2_ans);

        AOC::submitAnswer(2025, 5, 1, part1_ans);
        AOC::submitAnswer(2025, 5, 2, part2_ans);

    } catch (const std::exception& e) {
        std::cerr << "Error fetching input: " << e.what() << "\n";
        return 1;
    }

    return 0;
}