cmake_minimum_required (VERSION 2.8)

project(Hello)

# Find libcurl
find_package(CURL REQUIRED)

set(X86_COMPILE OFF)
set(INCLUDE_DEBUG_SYMBOLS ON)

if(INCLUDE_DEBUG_SYMBOLS)
    #make debug 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

if(X86_COMPILE)
    # Set the path to the directory containing the required dynamic linker library
    link_directories("/usr/aarch64-linux-gnu/lib/")

    # Set the linker flags to use the specific dynamic linker library
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--dynamic-linker=/usr/aarch64-linux-gnu/lib/ld-linux-aarch64.so.1")
endif()

ADD_EXECUTABLE(main main.cpp)
TARGET_LINK_LIBRARIES(main)