#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "AOC.hpp"

std::vector<std::pair<long long, long long> > parseRanges(
    const std::string_view input) {
    std::vector<std::pair<long long, long long> > result;
    std::istringstream ss((input.data()));
    std::string token;

    while (std::getline(ss, token, ',')) {
        if (token.empty()) continue;
        const auto dash = token.find('-');
        long long start = std::stoll(token.substr(0, dash));
        long long end = std::stoll(token.substr(dash + 1));
        result.emplace_back(start, end);
    }

    return result;
}

bool isRepeatedAtLeastTwice(const long long n) {
    const std::string s = std::to_string(n);
    const auto L = s.size();

    for (int l = 1; l <= L / 2; ++l) {
        if (L % l != 0) continue;

        const std::string base = s.substr(0, l);
        const size_t k = L / l;

        if (k < 2) continue;

        std::string rebuilt;
        rebuilt.reserve(L);
        for (int i = 0; i < k; ++i) rebuilt += base;

        if (rebuilt == s) return true;
    }

    return false;
}

bool isDoubleRepeated(const long long n) {
    const std::string s = std::to_string(n);
    const size_t L = s.size();

    if (L % 2 != 0) return false;

    const size_t half = L / 2;
    return s.substr(0, half) == s.substr(half);
}

long long part1(const std::string_view input) {
    auto ranges = parseRanges(input);
    long long sum = 0;

    for (auto [start, end] : ranges) {
        for (long long n = start; n <= end; ++n) {
            if (isDoubleRepeated(n)) {
                sum += n;
            }
        }
    }

    return sum;
}

long long part2(std::string_view input) {
    auto ranges = parseRanges(input);
    long long sum = 0;

    for (auto [start, end] : ranges) {
        for (long long n = start; n <= end; ++n) {
            if (isRepeatedAtLeastTwice(n)) {
                sum += n;
            }
        }
    }

    return sum;
}

int main() {
    try {
        const std::string input = AOC::fetchRawInput(2025, 2);
        std::println("Part 1 ans : {}", part1(input));
        std::println("Part 2 ans : {}", part2(input));
    } catch (const std::exception& e) {
        std::cerr << "Error fetching input: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
