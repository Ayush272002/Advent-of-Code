#include <iostream>
#include <print>
#include <vector>

#include "AOC.hpp"

int bestForLine(const std::string_view s) {
    const std::size_t n = s.size();
    int best = 0;

    std::vector<int> suf(n, -1);
    int maxRight = -1;

    for (std::size_t i = n; i-- > 0;) {
        suf[i] = maxRight;
        maxRight = std::max(maxRight, s[i] - '0');
    }

    for (std::size_t i = 0; i + 1 < n; ++i) {
        const int a = s[i] - '0';
        const int b = suf[i];

        if (b != -1) best = std::max(best, 10 * a + b);
    }

    return best;
}

std::string best12(const std::string_view s) {
    constexpr int K = 12;
    const std::size_t n = s.size();
    const std::size_t toRemove = n - K;

    std::string stack;
    stack.reserve(n);

    std::size_t removed = 0;

    for (const char c : s) {
        while (!stack.empty() && removed < toRemove && stack.back() < c) {
            stack.pop_back();
            ++removed;
        }

        stack.push_back(c);
    }

    return stack.substr(0, K);
}

int part1(const std::vector<std::string>& input) {
    int sum = 0;
    for (const auto& line : input) {
        sum += bestForLine(line);
    }

    return sum;
}

long long part2(const std::vector<std::string>& input) {
    long long sum = 0;

    for (const auto& line : input) {
        std::string best = best12(line);
        sum += std::stoll(best);
    }

    return sum;
}

int main() {
    try {
        const std::vector<std::string> input =
            AOC::fetchAOCInputVector(2025, 3);

        std::string part1_ans = std::to_string(part1(input));
        std::string part2_ans = std::to_string(part2(input));

        std::println("Part 1 ans : {}", part1_ans);
        std::println("Part 2 ans : {}", part2_ans);

        AOC::submitAnswer(2025, 3, 1, part1_ans);
        AOC::submitAnswer(2025, 3, 2, part2_ans);
    } catch (const std::exception& e) {
        std::cerr << "Error fetching input: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
