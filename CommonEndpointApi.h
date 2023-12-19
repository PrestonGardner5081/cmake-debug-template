#pragma once
#include <string>
#include <curl/curl.h>

class CommonEndpointApi
{
private:
    struct ResponseData {
        char* data;
        size_t size;
        long response_code;
    };

    std::string resourceUrl;
    std::string authUrl;
    CURL *resourceCurlPtr = nullptr;
    CURL *authCurlPtr = nullptr;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    static size_t AuthCallback(void* contents, size_t size, size_t nmemb, void* userp);


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