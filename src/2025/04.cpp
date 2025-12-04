#include <iostream>
#include <print>
#include <vector>

#include "AOC.hpp"

const std::vector<std::vector<int>> dir = {{1, 0}, {-1, 0}, {0, 1},  {0, -1},
                                           {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

bool isValid(const int r, const int c, const std::vector<std::string>& input) {
    int count = 0;
    for (const auto& d : dir) {
        const int x = r + d[0];
        const int y = c + d[1];

        if (x < 0 || y < 0 || x >= input.size() || y >= input[0].size())
            continue;

        if (input[x][y] == '@') count++;
    }

    return count < 4;
}

int part1(const std::vector<std::string>& input) {
    int ans = 0;
    for (int i = 0; i < input.size(); i++) {
        for (int j = 0; j < input[0].size(); j++) {
            if (input[i][j] == '@' && isValid(i, j, input)) ans++;
        }
    }

    return ans;
}

int part2(std::vector<std::string>& input) {
    int ans = 0;

    while (true) {
        std::vector<std::pair<int, int>> toRemove;

        for (int i = 0; i < input.size(); i++) {
            for (int j = 0; j < input[0].size(); j++) {
                if (input[i][j] == '@' && isValid(i, j, input))
                    toRemove.emplace_back(i, j);
            }
        }

        if (toRemove.empty()) break;

        for (auto&[fst, snd] : toRemove) {
            input[fst][snd] = '.';
        }

        ans += static_cast<int>(toRemove.size());
    }

    return ans;
}

int main() {
    try {
        std::vector<std::string> input = AOC::fetchAOCInputVector(2025, 4);

        std::string part1_ans = std::to_string(part1(input));
        std::string part2_ans = std::to_string(part2(input));

        std::println("Part 1 ans : {}", part1_ans);
        std::println("Part 2 ans : {}", part2_ans);

        AOC::submitAnswer(2025, 4, 1, part1_ans);
        AOC::submitAnswer(2025, 4, 2, part2_ans);

    } catch (const std::exception& e) {
        std::cerr << "Error fetching input: " << e.what() << "\n";
        return 1;
    }

    return 0;
}