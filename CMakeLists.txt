cmake_minimum_required (VERSION 2.8)

project(Hello)

ADD_LIBRARY(Hello Hello.h Hello.cpp)
ADD_EXECUTABLE(main main.cpp)
TARGET_LINK_LIBRARIES(main Hello)