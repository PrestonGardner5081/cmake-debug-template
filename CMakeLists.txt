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

ADD_EXECUTABLE(main main.cpp CommonEndpointApi.cpp)
TARGET_LINK_LIBRARIES(main ${CURL_LIBRARIES})