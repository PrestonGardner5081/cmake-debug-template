cmake_minimum_required(VERSION 3.20)

project(Hello LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(X86_COMPILE OFF)
set(INCLUDE_DEBUG_SYMBOLS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_PREFIX_PATH "/usr/aarch64-linux-gnu/")

# Find libcurl
find_package(CURL REQUIRED)

include_directories(${CURL_INCLUDE_DIRS})

if(INCLUDE_DEBUG_SYMBOLS)
    #make debug 
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

if(X86_COMPILE)
    # Set the path to the directory containing the required dynamic linker library
    link_directories("/usr/aarch64-linux-gnu/lib/")

    # Set the linker flags to use the specific dynamic linker library
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--dynamic-linker=/usr/aarch64-linux-gnu/lib/ld-linux-aarch64.so.1")
endif()

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/json)

#Generate shared library as libCommonEndpointApi.so
set(SOURCES src/CommonEndpointApi.cpp)
add_library(CommonEndpointApi SHARED ${SOURCES})
TARGET_LINK_LIBRARIES(CommonEndpointApi PRIVATE ${CURL_LIBRARIES} nlohmann_json::nlohmann_json)

#Swig config
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
find_package(Python COMPONENTS Interpreter REQUIRED)
find_package(Python COMPONENTS Development REQUIRED)
include_directories(${Python_INCLUDE_DIRS})
include_directories("./src")
set_property(SOURCE src/common_endpoint_api.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE src/common_endpoint_api.i PROPERTY SWIG_FLAGS "-includeall")
set_property(SOURCE src/common_endpoint_api.i PROPERTY SWIG_MODULE_NAME common_endpoint_api)
swig_add_library(common_endpoint_api TYPE SHARED LANGUAGE python SOURCES src/common_endpoint_api.i)
swig_link_libraries(common_endpoint_api CommonEndpointApi)

#Test libCommonEndpointApi.so with main
ADD_EXECUTABLE(main src/main.cpp)
TARGET_LINK_LIBRARIES(main PRIVATE ${CURL_LIBRARIES} nlohmann_json::nlohmann_json CommonEndpointApi)

#JWT setup:
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/cpp-jwt)

# ADD_EXECUTABLE(main main.cpp CommonEndpointApi.cpp)
# TARGET_LINK_LIBRARIES(main PRIVATE ${CURL_LIBRARIES} nlohmann_json::nlohmann_json cpp-jwt)


