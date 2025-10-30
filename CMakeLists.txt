cmake_minimum_required(VERSION 3.14)
project(spotcli)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(
  main src/main.cpp
)

