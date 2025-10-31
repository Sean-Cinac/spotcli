cmake_minimum_required(VERSION 3.14)
project(spotcli)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)
include_directories(/usr/include/jsoncpp)

add_executable(main
    src/main.cpp
    src/playlist.cpp
)

find_package(CURL REQUIRED)
target_link_libraries(main PRIVATE CURL::libcurl)

target_link_libraries(main PRIVATE /usr/lib/libjsoncpp.so)
