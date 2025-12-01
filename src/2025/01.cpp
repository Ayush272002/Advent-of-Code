#include <iostream>
#include <string>
#include <vector>
#include "AOC.hpp"

int part1(std::vector<std::string> &combinations) {
    int zeroCount = 0;

    int curr = 50;

    for (std::string &s: combinations) {
        const char dir = s[0];
        const int num = std::stoi(s.substr(1));

        const int mag = (dir == 'L') ? -num : num;

        curr = (curr + mag) % 100;
        if (curr < 0) curr += 100;

        if (curr == 0) zeroCount++;
    }

    return zeroCount;
}

int part2(std::vector<std::string> &combinations) {
    int zeroCount = 0;

    int curr = 50;

    for (std::string &s: combinations) {
        const char dir = s[0];
        const int mag = std::stoi(s.substr(1));
        const int steps = (dir == 'R') ? 1 : -1;

        int k0;

        if (dir == 'R') k0 = (100 - curr) % 100;
        else k0 = curr % 100;

        int hits = 0;
        if (k0 == 0) hits = mag / 100;
        else if (k0 <= mag) hits = 1 + (mag - k0) / 100;

        zeroCount += hits;
        curr = (curr + steps * mag) % 100;
        if (curr < 0) curr += 100;
    }

    return zeroCount;
}


int main() {
    try {
        std::vector<std::string> input = AOC::fetchAOCInput(2025, 1);
        std::println("Part 1 ans : {}", part1(input));
        std::println("Part 2 ans : {}", part2(input));

    } catch (const std::exception &e) {
        std::cerr << "Error fetching input: " << e.what() << "\n";
        return 1;
    }

    return 0;
}