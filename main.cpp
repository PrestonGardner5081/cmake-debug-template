#include <errno.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <curl/curl.h>

#define DEBUG_MODE

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    printf("%.*s", (int)realsize, (char*)contents);
    std::string strContents = reinterpret_cast<const char*>(contents); 
    std::cout << " <- recieved from server"<< std::endl; //flush 

    return realsize;
}

int main() {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    
    if(curl) {

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); //set function to use on get

        // curl_easy_setopt(curl, CURLOPT_URL, "https://trident.sensorops.io/api/system/health");
        curl_easy_setopt(curl, CURLOPT_URL, "https://158.101.127.192:8443");
        curl_easy_setopt(curl, CURLOPT_CAINFO, "./test_server.cert"); //use test cert 
        // curl_easy_setopt(curl, CURLOPT_CAPATH, "./");//can use to set cert path 
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL); //use all ssl features

    #ifdef DEBUG_MODE
        #define SKIP_HOSTNAME_VERIFICATION //FIXME, using for debug purposes only
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //turn on verbose mode

    #endif
    #ifdef SKIP_HOSTNAME_VERIFICATION
    /*
        * If the site you are connecting to uses a different host name that what
        * they have mentioned in their server certificate's commonName (or
        * subjectAltName) fields, libcurl will refuse to connect. You can skip
        * this check, but this will make the connection less secure.
        */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    #endif
        /* cache the CA cert bundle in memory for a week */
        // curl_easy_setopt(curl, CURLOPT_CA_TIMEOUT, 604800L);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */ 
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        //POST JSON
        
        curl_easy_setopt(curl, CURLOPT_POST, 1L); // Set the request type to POST

        
        const char *post_data = "{\"test_key\": \"test_value\"}"; // Set the POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);

        
        struct curl_slist *headers = NULL; 
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); // Set the Content-Type header for JSON

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        //DO ANOTHER GET 
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);    

        res = curl_easy_perform(curl);
        /* Check for errors */ 
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}