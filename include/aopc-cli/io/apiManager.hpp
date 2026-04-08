#pragma once

#include <curl/curl.h>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


class APIManager {
    public:
        APIManager(const std::string& apiURL) : url(apiURL) {};

        bool performCurlRequest();
        bool parseJsonResponse();

        json getJsonResponse() const;

    private:
        const std::string url;
        std::string response;
        json jsonResponse;

        static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userData);
};