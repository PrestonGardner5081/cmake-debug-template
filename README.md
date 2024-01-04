# rest_api
This is a proof of concept for a a common API that can be called to retrieve and post data to Command Console and Helios endpoints. The API uses the host's keystore to validate an endpoint's PKI authenticity.

The API is written in C++ and generates a shared library as well as a python wrapper. These outputs are compiled for arm64.  

### Prequisites
- Swig: software which generates python wrapper 
    - `apt install swig`     

### Build
From the `rest_api` directory run the command:
    
    cmake . -B./build && make -C ./build

The output of this command in `rest_api/build` will be:
- libCommonEndpointApi.so: a C/C++ shared library build of the api
- main: an executable that will test the C/C++ build
- _common_endpoints_api.so: a shared library dependent on libCommonEndpointApi.so which acts as a bridge between the C/C++ code and the python wrapper
- common_endpoint_api.py: a python wrapper which allows a python file to make calls to the api