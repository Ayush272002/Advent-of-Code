#include "AOC.hpp"

#include <curl/curl.h>

#include <algorithm>
#include <dotenv/dotenv.hpp>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace AOC {
    static std::optional<std::string> loadAOCSession(
        const std::string& filename = ".env") {
        dotenv::load(filename);
        auto sessionOpt = dotenv::get<std::string>("AOC_SESSION");
        return sessionOpt;
    }

    static size_t WriteCallback(void* contents, const size_t size,
                                const size_t nmemb, void* userp) {
        std::string& response = *static_cast<std::string*>(userp);
        response.append(static_cast<char*>(contents), size * nmemb);
        return size * nmemb;
    }

    std::string fetchRawInput(const int year, const int day) {
        const auto sessionOpt = loadAOCSession("../.env");
        if (!sessionOpt) {
            throw std::runtime_error("AOC_SESSION not found in .env");
        }
        const std::string& session = *sessionOpt;

        const std::string url =
            std::format("https://adventofcode.com/{}/day/{}/input", year, day);

        CURL* curl = curl_easy_init();
        if (!curl) throw std::runtime_error("Failed to initialize curl.");

        std::string response;
        const std::string cookieHeader = "session=" + session;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_COOKIE, cookieHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        const CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error("Failed to fetch input from AoC.");
        }

        return response;
    }

    std::vector<std::string> fetchAOCInputVector(const int year,
                                                 const int day) {
        const std::string rawInput = fetchRawInput(year, day);
        std::istringstream iss(rawInput);

        std::vector<std::string> lines;
        std::string line;

        while (std::getline(iss, line)) {
            if (!line.empty()) {
                lines.push_back(std::move(line));
            }
        }

        return lines;
    }

    void submitAnswer(int year, int day, int part, std::string_view answer) {
        const auto sessionOpt = loadAOCSession("../.env");

        if (!sessionOpt) {
            throw std::runtime_error("AOC_SESSION not found in .env");
        }

        const std::string& session = *sessionOpt;

        const std::string url =
            std::format("https://adventofcode.com/{}/day/{}/answer", year, day);

        CURL* curl = curl_easy_init();
        if (!curl) throw std::runtime_error("Failed to initialize curl.");

        std::string response;
        const std::string cookieHeader = "session=" + session;

        const std::string postFields =
            std::format("level={}&answer={}", part, answer);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_COOKIE, cookieHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

        curl_easy_setopt(
            curl, CURLOPT_USERAGENT,
            "C++/curl AdventOfCodeClient (github.com/Ayush272002)");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        const CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error("Failed to submit answer.");
        }

        std::string lower = response;
        std::ranges::transform(lower, lower.begin(), ::tolower);

        if (lower.contains("that's the right answer")) {
            std::println("correct");
            return;
        }
        if (lower.contains("that's not the right answer")) {
            std::println("incorrect");
            return;
        }
        if (lower.contains("you gave an answer too recently")) {
            std::println("too-recent");
            return;
        }
        if (lower.contains("don't seem to be solving the right level")) {
            std::println("wrong-level");
            return;
        }
        if (lower.contains("please don't repeatedly request this endpoint")) {
            std::println("too-early");
            return;
        }

        std::println("unknown");
    }
}  // namespace AOC
