#pragma once
#include <string>
#include <curl/curl.h>

class CommonEndpointApi
{
private:
    std::string resourceUrl;
    CURL *connectionInstancePtr = nullptr;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

public:
    CommonEndpointApi();
    CommonEndpointApi(std::string resourceUrl);
    ~CommonEndpointApi();

    void setResourceUrl(std::string resourceUrl);
    
    std::string getResourceUrl();

    bool performResourceUrlGet();

    bool performResourceUrlPost(const char* data);
};