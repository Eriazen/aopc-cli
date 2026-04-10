#include "aopc-cli/io/apiManager.hpp"
#include <iostream>


size_t APIManager::writeCallback(void* contents, size_t size, size_t nmemb, void* userData) {
    size_t totalSize = size * nmemb;
    std::string* str = static_cast<std::string*>(userData);
    str->append(static_cast<char*>(contents), totalSize);

    return totalSize;
}

bool APIManager::performCurlRequest() {
    CURL *curl = curl_easy_init();

    if (!curl) {
        std::cerr << "Error: Failed to initialize CURL" << std::endl;
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "Error: curl_easy_perform() failed with " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    return true;
}

bool APIManager::parseJsonResponse() {
    try {
        m_jsonResponse = json::parse(m_response);

        return true;
    } catch (json::parse_error& e) {
        std::cerr << "Error: Couldn't parse curl response with exception '" << e.what() << "'." << std::endl;
        return false;
    }
}