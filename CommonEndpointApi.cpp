#include "CommonEndpointApi.h"
#include <iostream>
#define DEBUG_MODE //FIXME remove for prod

CommonEndpointApi::CommonEndpointApi()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    this->connectionInstancePtr = curl_easy_init();
    curl_easy_setopt(this->connectionInstancePtr, CURLOPT_USE_SSL, CURLUSESSL_ALL); //use all ssl features
    
    #ifdef DEBUG_MODE
        curl_easy_setopt(this->connectionInstancePtr, CURLOPT_SSL_VERIFYHOST, 0L); //turn off SSL hostname verification
        // curl_easy_setopt(this->connectionInstancePtr, CURLOPT_VERBOSE, 1L); //turn on verbose mode
        curl_easy_setopt(connectionInstancePtr, CURLOPT_CAINFO, "./test_server.cert"); //use test cert 
    #endif
}

CommonEndpointApi::CommonEndpointApi(std::string resourceUrl) : CommonEndpointApi(){
    this->resourceUrl = resourceUrl;
    curl_easy_setopt(this->connectionInstancePtr, CURLOPT_URL, resourceUrl.c_str());
}

CommonEndpointApi::~CommonEndpointApi()
{
    curl_easy_cleanup(this->connectionInstancePtr);
    curl_global_cleanup();
}

void CommonEndpointApi::setResourceUrl(std::string resourceUrl){
    this->resourceUrl = resourceUrl;
    curl_easy_setopt(this->connectionInstancePtr, CURLOPT_URL, resourceUrl.c_str());
}

std::string CommonEndpointApi::getResourceUrl(){
    return this->resourceUrl;
}

bool CommonEndpointApi::performResourceUrlGet(){
    if (this->connectionInstancePtr == nullptr || this->resourceUrl.empty()){
        fprintf(stderr, "CommonEndpointApi::performResourceUrlGet() CommonEndpointApi has not been fully initialized");
        return false;
    }

    curl_easy_setopt(connectionInstancePtr, CURLOPT_HTTPGET, 1L);    
    curl_easy_setopt(connectionInstancePtr, CURLOPT_WRITEFUNCTION, CommonEndpointApi::WriteCallback); //set function to use on get
    CURLcode res = curl_easy_perform(this->connectionInstancePtr);

    if(res != CURLE_OK){
        fprintf(stderr, "CommonEndpointApi::performResourceUrlGet() curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return false;
    }

    return true;
};

bool CommonEndpointApi::performResourceUrlPost(const char* data){
    if (this->connectionInstancePtr == nullptr || this->resourceUrl.empty()){
        fprintf(stderr, "CommonEndpointApi::performResourceUrlPost() CommonEndpointApi has not been fully initialized");
        return false;
    }

    curl_easy_setopt(connectionInstancePtr, CURLOPT_POST, 1L); // Set the request type to POST
    curl_easy_setopt(connectionInstancePtr, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(connectionInstancePtr, CURLOPT_WRITEFUNCTION, CommonEndpointApi::WriteCallback); //set callback for handling response
    
    struct curl_slist *headers = NULL; 
    headers = curl_slist_append(headers, "Content-Type: application/json"); //FIXME, maybe use enum to 
    curl_easy_setopt(connectionInstancePtr, CURLOPT_HTTPHEADER, headers); // Set the Content-Type header for JSON

    CURLcode res = curl_easy_perform(this->connectionInstancePtr);

    if(res != CURLE_OK){
        fprintf(stderr, "CommonEndpointApi::performResourceUrlGet() curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return false;
    }

    return true;
}

size_t CommonEndpointApi::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp){
    size_t realsize = size * nmemb;
    printf("%.*s", (int)realsize, (char*)contents);
    std::string strContents = reinterpret_cast<const char*>(contents); 
    std::cout << " <- recieved from server"<< std::endl; //curl already sends response to standard output, we just need to flush it 

    return realsize;
}