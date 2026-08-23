cmake_minimum_required(VERSION 3.16)
project(ReactorHttpServer VERSION 0.2.0 LANGUAGES CXX)

# modern c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include directory for headers
include_directories(include)

# source files
set(SOURCES
    src/main.cpp
    src/server.cpp
    src/thread_pool.cpp
)

add_executable(http_server ${SOURCES})

# link pthread if needed on POSIX platforms
find_package(Threads REQUIRED)
target_link_libraries(http_server PRIVATE Threads::Threads)
