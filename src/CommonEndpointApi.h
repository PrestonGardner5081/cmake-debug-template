#pragma once
#include <curl/curl.h>
#include <string>

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

    bool authenticateWithUserPassword(std::string username, std::string password);

    bool performResourceUrlGet();

    bool performResourceUrlPost(const char* data);
};