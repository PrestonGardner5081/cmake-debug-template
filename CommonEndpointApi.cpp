#include "CommonEndpointApi.h"
#include <cstring>

#define DEBUG_MODE //FIXME remove for prod
#define USE_TEST_SERVER //FIXME remove for prod

using json = nlohmann::json;

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

    curl_easy_setopt(authCurlPtr, CURLOPT_HTTPGET, 1L);//set request type to GET
    
    struct ResponseData responseHeader = { .data = nullptr, .size = 0};
    curl_easy_setopt(authCurlPtr, CURLOPT_HEADERDATA, &responseHeader);//pass struct to header callback 
    curl_easy_setopt(authCurlPtr, CURLOPT_HEADERFUNCTION, CommonEndpointApi::HeaderCallback); //set function to handle response header

    struct ResponseData responseBody = { .data = nullptr, .size = 0};
    curl_easy_setopt(authCurlPtr, CURLOPT_WRITEDATA, &responseBody);//pass struct to write callback 
    curl_easy_setopt(authCurlPtr, CURLOPT_WRITEFUNCTION, CommonEndpointApi::WriteCallback); //set function to handle response body

    std::string usrpwd_str = userName + ":" + password;
    
    curl_easy_setopt(authCurlPtr, CURLOPT_USERPWD, usrpwd_str.c_str()); //set authorization header
    CURLcode res = curl_easy_perform(this->authCurlPtr);

    if(res != CURLE_OK){
        fprintf(stderr, "CommonEndpointApi::authenticateWithUserPassword() curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        
        free(responseBody.data);
        return false;
    }
    
    long responseCode = 0L; 
    curl_easy_getinfo(authCurlPtr, CURLINFO_RESPONSE_CODE, &responseCode); //get response code from last request

    if (responseCode == 200L && responseBody.data != nullptr){
        std::string token;

        if (!getTokenFromResponse(responseBody.data, token)){
            free(responseBody.data);
            return false;
        }
        //TODO store token or something
    
        #ifdef DEBUG_MODE
            std::cout << "Successful GET on " << this->authUrl <<  "\n" << std::endl;
        #endif

        free(responseBody.data);
        return true;
    }
    else{
        fprintf(stderr, "CommonEndpointApi::authenticateWithUserPassword() Failed to get token from server\n");
        free(responseBody.data);
        return false;
    }
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

    struct ResponseData responseBody = { .data = nullptr, .size = 0};
    curl_easy_setopt(resourceCurlPtr, CURLOPT_WRITEDATA, &responseBody);//pass struct to write callback 
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

    free(responseBody.data);
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

    struct ResponseData responseBody = { .data = nullptr, .size = 0};
    curl_easy_setopt(resourceCurlPtr, CURLOPT_WRITEDATA, &responseBody);//pass struct to write callback 
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

    free(responseBody.data);
    return true;
}


/*Static helper functions*/

size_t CommonEndpointApi::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp){
    size_t realsize = size * nmemb;
    struct ResponseData* mem = reinterpret_cast<struct ResponseData*>(userp);
    mem->data = reinterpret_cast<char*>(std::malloc(realsize + 1));

    if (mem->data == nullptr) {
        fprintf(stderr, "CommonEndpointApi::WriteCallback() failed to realloc mem->data\n");
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

size_t CommonEndpointApi::HeaderCallback(char *buffer, size_t size, size_t nitems, void *userdata){ //We probably shouldnt reuse WriteCallback even though it is similar
    size_t realsize = size * nitems;
    struct ResponseData* mem = reinterpret_cast<struct ResponseData*>(userdata);
    mem->data = reinterpret_cast<char*>(std::malloc(realsize + 1));

    if (mem->data == nullptr) {
        fprintf(stderr, "CommonEndpointApi::HeaderCallback() failed to realloc mem->data\n");
        return 0;
    }

    std::memcpy(mem->data, buffer, realsize); //copy data to struct
    mem->data[realsize] = '\0'; //add null termination
    mem->size = realsize;

    #ifdef DEBUG_MODE
        std::string headerStr = mem->data;
        std::cout << "Headers: " << headerStr << std::endl;
    #endif

    return realsize;
}

bool CommonEndpointApi::getAllKeys(const json& jsonObject, std::vector<std::string>& keys) {
    if (jsonObject.is_object()){
        for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
            keys.push_back(it.key());
        }
        return true;
    }

    fprintf(stderr, "CommonEndpointApi::getAllKeys() Parameter jsonObject was not a json object\n");
    return false;
}

bool CommonEndpointApi::getTokenFromResponse(char* data, std::string& token){
    try {
        json responseBodyJson = json::parse(data);
        std::vector<std::string> keys;
        if (!getAllKeys(responseBodyJson, keys))
            return false; 

        if (std::find(keys.begin(), keys.end(), "tokens") != keys.end()){
            json tokens = responseBodyJson["tokens"];
            
            if (!tokens.is_array()){
                fprintf(stderr, "CommonEndpointApi::getTokenFromResponse() value for 'tokens' was not an array\n");
                return false;
            }
                
            if(!tokens[0].is_string()){
                fprintf(stderr, "CommonEndpointApi::getTokenFromResponse() token was not a string\n");
                return false;
            }

            token = tokens[0].get<std::string>();

            std::cout << "Debugging: FIXME remove this line" << std::endl;//FIXME
        }
        else{
            fprintf(stderr, "CommonEndpointApi::getTokenFromResponse() No token found\n");
            return false;
        }
        
    } catch (const json::parse_error& e) {
        // Handle the parse error
        std::cerr << "CommonEndpointApi::authenticateWithUserPassword() Error parsing JSON: " << e.what() << " at position " << e.byte << std::endl;
        return false;
    }

    return true;
}
