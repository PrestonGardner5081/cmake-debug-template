#include "CommonEndpointApi.h"

int main() {
    // CommonEndpointApi api = CommonEndpointApi("https://158.101.127.192:8443/simple_storage", "https://172.18.96.1:8443/auth");//https://trident.sensorops.io/api/auth/token
    CommonEndpointApi api = CommonEndpointApi("https://158.101.127.192:8443/simple_storage", "https://trident.sensorops.io/api/auth/token");

    api.authenticateWithUserPassword("preston", "openwater");
    // // api.authenticateWithUserPassword("badUser", "pswd98765456831545524253");
    // api.performResourceUrlGet();

    // std::string post_data = "{\"test_key\": \"test_value\"}"; // Set the POST data
    // api.setResourceUrl("https://158.101.127.192:8443/simple_storage");
    // api.performResourceUrlPost(post_data);    
    // api.performResourceUrlGet();

    // api.setResourceUrl("https://trident.sensorops.io/api/system/health");
    // api.performResourceUrlGet();
    return 0;
}