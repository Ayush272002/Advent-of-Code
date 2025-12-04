#pragma once
#include <string>
#include <vector>

namespace AOC {
    std::string fetchRawInput(int year, int day);
    std::vector<std::string> fetchAOCInputVector(int year, int day);
    void submitAnswer(int year, int day, int part, std::string_view answer);
}  // namespace AOC
