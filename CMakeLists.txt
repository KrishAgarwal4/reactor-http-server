cmake_minimum_required(VERSION 3.16)
project(ReactorHttpServer VERSION 0.1.0 LANGUAGES CXX)

# using modern c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include headers
include_directories(include)

# source files
set(SOURCES
    src/main.cpp
    src/server.cpp
)

add_executable(http_server ${SOURCES})
