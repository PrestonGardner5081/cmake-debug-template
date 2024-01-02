// common_endpoint_api.i

%module common_endpoint_api
%{
#include "CommonEndpointApi.h"
%}

%include "std_string.i"

%typemap(in) std::string {
    $1 = PyString_AsString($input);
}

class CommonEndpointApi {
public:
    CommonEndpointApi();
    CommonEndpointApi(std::string resourceUrl, std::string authUrl);
    ~CommonEndpointApi();

    void initResourceConnection();
    void initAuthConnection();

    void setResourceUrl(std::string resourceUrl);
    void setAuthUrl(std::string authUrl);
    
    std::string getResourceUrl();
    std::string getAuthUrl();

    bool authenticateWithUserPassword(std::string username, std::string password);

    bool performResourceUrlGet();
    bool performResourceUrlPost(std::string data);
};