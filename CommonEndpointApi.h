#pragma once
#include <string>
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>

class CommonEndpointApi
{
private:
    struct ResponseData {
        char* data;
        size_t size;
    };

    std::string resourceUrl;
    std::string authUrl;
    CURL *resourceCurlPtr = nullptr;
    CURL *authCurlPtr = nullptr;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    static size_t HeaderCallback(char *buffer, size_t size, size_t nitems, void *userdata);
    static bool getAllKeys(const nlohmann::json& jsonObject, std::vector<std::string>& keys);
    static bool getTokenFromResponse(char* data, std::string& token);

public:
    void initResourceConnection();
    void initAuthConnection();

    CommonEndpointApi();
    CommonEndpointApi(std::string resourceUrl, std::string authUrl);
    ~CommonEndpointApi();

    void setResourceUrl(std::string resourceUrl);

    void setAuthUrl(std::string authUrl);
    
    std::string getResourceUrl();

    std::string getAuthUrl();

    bool authenticateWithUserPassword(std::string userName, std::string password);

    bool performResourceUrlGet();

    bool performResourceUrlPost(const char* data);
};