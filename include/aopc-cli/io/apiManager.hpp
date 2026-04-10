#pragma once

#include <curl/curl.h>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


class APIManager {
    public:
        APIManager(const std::string& apiURL) : m_url(apiURL) {};

        bool performCurlRequest();
        bool parseJsonResponse();

        json getJsonResponse() const { return m_jsonResponse; };

    private:
        const std::string m_url;
        std::string m_response;
        json m_jsonResponse;

        static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userData);
};