cmake_minimum_required(VERSION 4.0)
project(AsciiArt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(AsciiArt src/main.cpp src/Image.cpp)

target_include_directories(AsciiArt PRIVATE external)