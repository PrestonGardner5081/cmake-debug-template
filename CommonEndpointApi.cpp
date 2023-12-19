#include "CommonEndpointApi.h"
#include <cstring>
#include <iostream>


#define DEBUG_MODE //FIXME remove for prod
#define USE_TEST_SERVER //FIXME remove for prod

void CommonEndpointApi::initResourceConnection(){
    this->resourceCurlPtr = curl_easy_init();
    curl_easy_setopt(this->resourceCurlPtr, CURLOPT_USE_SSL, CURLUSESSL_ALL); //use all ssl features

    #ifdef DEBUG_MODE
        curl_easy_setopt(this->resourceCurlPtr, CURLOPT_VERBOSE, 1L); //turn on verbose mode
    #endif
    #ifdef USE_TEST_SERVER
        curl_easy_setopt(this->resourceCurlPtr, CURLOPT_SSL_VERIFYHOST, 0L); //turn off SSL hostname verification
        curl_easy_setopt(this->resourceCurlPtr, CURLOPT_CAINFO, "./test_server.cert"); //use test cert 
    #endif
}

void CommonEndpointApi::initAuthConnection(){
    this->authCurlPtr = curl_easy_init();
    curl_easy_setopt(this->authCurlPtr, CURLOPT_USE_SSL, CURLUSESSL_ALL); //use all ssl features
    
    #ifdef DEBUG_MODE
        curl_easy_setopt(this->authCurlPtr, CURLOPT_VERBOSE, 1L); //turn on verbose mode
    #endif
    #ifdef USE_TEST_SERVER
        curl_easy_setopt(this->authCurlPtr, CURLOPT_SSL_VERIFYHOST, 0L); //turn off SSL hostname verification
        curl_easy_setopt(this->authCurlPtr, CURLOPT_CAINFO, "./test_server.cert"); //use test cert 
    #endif
}

CommonEndpointApi::CommonEndpointApi()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    this->initResourceConnection();
    this->initAuthConnection();
}

CommonEndpointApi::CommonEndpointApi(std::string resourceUrl, std::string authUrl) : CommonEndpointApi(){
    this->resourceUrl = resourceUrl;
    this->authUrl = authUrl;
    curl_easy_setopt(this->resourceCurlPtr, CURLOPT_URL, resourceUrl.c_str());
    curl_easy_setopt(this->authCurlPtr, CURLOPT_URL, authUrl.c_str());
}

CommonEndpointApi::~CommonEndpointApi()
{
    curl_easy_cleanup(this->resourceCurlPtr);
    this->resourceCurlPtr = nullptr;
    this->resourceUrl = "";
    curl_easy_cleanup(this->authCurlPtr);
    this->authCurlPtr = nullptr;
    this->authUrl = "";
    curl_global_cleanup();
}

void CommonEndpointApi::setResourceUrl(std::string resourceUrl){
    if (!this->resourceUrl.empty()){
        curl_easy_cleanup(this->resourceCurlPtr);
        this->resourceCurlPtr = nullptr;
        this->resourceUrl = "";

        this->initResourceConnection();
    }

    this->resourceUrl = resourceUrl;
    curl_easy_setopt(this->resourceCurlPtr, CURLOPT_URL, resourceUrl.c_str());
}

std::string CommonEndpointApi::getResourceUrl(){
    return this->resourceUrl;
}

void CommonEndpointApi::setAuthUrl(std::string authUrl){
    if (!this->authUrl.empty()){
        curl_easy_cleanup(this->authCurlPtr);
        this->authCurlPtr = nullptr;
        this->authUrl = "";

        this->initAuthConnection();
    }

    this->authUrl = authUrl;
    curl_easy_setopt(this->authCurlPtr, CURLOPT_URL, authUrl.c_str());
}

std::string CommonEndpointApi::getAuthUrl(){
    return this->authUrl;
}

bool CommonEndpointApi::authenticateWithUserPassword(std::string userName, std::string password){
    #ifdef DEBUG_MODE
        std::cout << "Attempting auth GET on " << this->authUrl <<  "\n";
    #endif

    if (this->authCurlPtr == nullptr || this->authUrl.empty()){
        fprintf(stderr, "CommonEndpointApi::authenticateWithUserPassword() CommonEndpointApi has not been fully initialized");
        return false;
    }

    curl_easy_setopt(authCurlPtr, CURLOPT_HTTPGET, 1L);//set type to GET
    
    struct ResponseData responseData = { .data = nullptr, .size = 0, .response_code = 0 };
    curl_easy_setopt(authCurlPtr, CURLOPT_WRITEDATA, &responseData);//pass struct to write callback 
    curl_easy_setopt(authCurlPtr, CURLOPT_WRITEFUNCTION, CommonEndpointApi::WriteCallback); //set function to use on get

    std::string usrpwd_str = userName + ":" + password;
    
    curl_easy_setopt(authCurlPtr, CURLOPT_USERPWD, usrpwd_str.c_str()); //set authorization header
    CURLcode res = curl_easy_perform(this->authCurlPtr);

    if(res != CURLE_OK){
        fprintf(stderr, "CommonEndpointApi::authenticateWithUserPassword() curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return false;
    }

    curl_easy_getinfo(authCurlPtr, CURLINFO_RESPONSE_CODE, &(responseData.response_code)); //get response code from last request

    if (responseData.response_code == 200L){

    }
    
    #ifdef DEBUG_MODE
        std::cout << "Successful GET on " << this->authUrl <<  "\n" << std::endl;
    #endif
    free(responseData.data);
    return true;
}

bool CommonEndpointApi::performResourceUrlGet(){
    #ifdef DEBUG_MODE
        std::cout << "Attempting GET on " << this->resourceUrl <<  "\n";
    #endif

    if (this->resourceCurlPtr == nullptr || this->resourceUrl.empty()){
        fprintf(stderr, "CommonEndpointApi::performResourceUrlGet() CommonEndpointApi has not been fully initialized");
        return false;
    }

    curl_easy_setopt(resourceCurlPtr, CURLOPT_HTTPGET, 1L);    

    struct ResponseData responseData = { .data = nullptr, .size = 0, .response_code = 0 };
    curl_easy_setopt(resourceCurlPtr, CURLOPT_WRITEDATA, &responseData);//pass struct to write callback 
    curl_easy_setopt(resourceCurlPtr, CURLOPT_WRITEFUNCTION, CommonEndpointApi::WriteCallback); //set function to use on get
    CURLcode res = curl_easy_perform(this->resourceCurlPtr);

    if(res != CURLE_OK){
        fprintf(stderr, "CommonEndpointApi::performResourceUrlGet() curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return false;
    }

    #ifdef DEBUG_MODE
        std::cout << "Successful GET on " << this->resourceUrl <<  "\n" << std::endl;
    #endif

    free(responseData.data);
    return true;
};

bool CommonEndpointApi::performResourceUrlPost(const char* data){
    #ifdef DEBUG_MODE
        std::cout << "Attempting POST on " << this->resourceUrl <<  "\n";
    #endif

    if (this->resourceCurlPtr == nullptr || this->resourceUrl.empty()){
        fprintf(stderr, "CommonEndpointApi::performResourceUrlPost() CommonEndpointApi has not been fully initialized");
        return false;
    }

    curl_easy_setopt(resourceCurlPtr, CURLOPT_POST, 1L); // Set the request type to POST
    curl_easy_setopt(resourceCurlPtr, CURLOPT_POSTFIELDS, data);

    struct ResponseData responseData = { .data = nullptr, .size = 0, .response_code = 0 };
    curl_easy_setopt(resourceCurlPtr, CURLOPT_WRITEDATA, &responseData);//pass struct to write callback 
    curl_easy_setopt(resourceCurlPtr, CURLOPT_WRITEFUNCTION, CommonEndpointApi::WriteCallback); //set callback for handling response
    
    struct curl_slist *headers = NULL; 
    headers = curl_slist_append(headers, "Content-Type: application/json"); //FIXME, maybe use enum to 
    curl_easy_setopt(resourceCurlPtr, CURLOPT_HTTPHEADER, headers); // Set the Content-Type header for JSON

    CURLcode res = curl_easy_perform(this->resourceCurlPtr);

    if(res != CURLE_OK){
        fprintf(stderr, "CommonEndpointApi::performResourceUrlPost() curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return false;
    }

    #ifdef DEBUG_MODE
        std::cout << "Successful POST on " << this->resourceUrl << "\n" << std::endl;
    #endif

    free(responseData.data);
    return true;
}

size_t CommonEndpointApi::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp){
    size_t realsize = size * nmemb;
    struct ResponseData* mem = reinterpret_cast<struct ResponseData*>(userp);
    mem->data = reinterpret_cast<char*>(std::malloc(realsize + 1));

    if (mem->data == nullptr) {
        fprintf(stderr, "CommonEndpointApi::AuthCallback() failed to realloc mem->data\n");
        return 0;
    }

    std::memcpy(mem->data, contents, realsize); //copy data to struct
    mem->data[realsize] = '\0'; //add null termination
    mem->size = realsize;

    #ifdef DEBUG_MODE
        std::string responseStr = mem->data;
        std::cout << "Server says:" << responseStr << std::endl;
    #endif

    return realsize;
}