#include "AOC.hpp"
#include <curl/curl.h>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>

namespace AOC {
    static std::optional<std::string> loadAOCSession(const std::string &filename = ".env") {
        std::filesystem::path current = std::filesystem::current_path();

        while (true) {
            std::filesystem::path envPath = current / filename;
            if (std::filesystem::exists(envPath)) {
                std::ifstream envFile(envPath);
                std::string line;
                while (std::getline(envFile, line)) {
                    line.erase(0, line.find_first_not_of(" \t"));
                    line.erase(line.find_last_not_of(" \t\r\n") + 1);

                    if (line.empty() || line[0] == '#') continue;

                    auto pos = line.find('=');
                    if (pos == std::string::npos) continue;

                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);

                    if (!value.empty() && value.front() == '"' && value.back() == '"')
                        value = value.substr(1, value.size() - 2);

                    if (key == "AOC_SESSION")
                        return value;
                }
            }

            if (current.has_parent_path())
                current = current.parent_path();
            else
                break;
        }

        return std::nullopt;
    }

    static size_t WriteCallback(void *contents, const size_t size, const size_t nmemb, void *userp) {
        std::string &response = *static_cast<std::string *>(userp);
        response.append(static_cast<char *>(contents), size * nmemb);
        return size * nmemb;
    }

    static std::string fetchRawInput(const int year, const int day) {
        auto sessionOpt = loadAOCSession();
        if (!sessionOpt) {
            throw std::runtime_error("AOC_SESSION not found in .env");
        }
        const std::string &session = *sessionOpt;

        const std::string url = "https://adventofcode.com/" + std::to_string(year) +
                                "/day/" + std::to_string(day) + "/input";

        CURL *curl = curl_easy_init();
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

    std::vector<std::string> fetchAOCInput(const int year, const int day) {
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
}
