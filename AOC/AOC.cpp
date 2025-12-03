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
        dotenv::load("../.env");
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
        const auto sessionOpt = loadAOCSession();
        if (!sessionOpt) {
            throw std::runtime_error("AOC_SESSION not found in .env");
        }
        const std::string& session = *sessionOpt;

        const std::string url = "https://adventofcode.com/" +
                                std::to_string(year) + "/day/" +
                                std::to_string(day) + "/input";

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
}  // namespace AOC
