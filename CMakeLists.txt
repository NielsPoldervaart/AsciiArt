cmake_minimum_required(VERSION 4.0)
project(AsciiArt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(AsciiArt src/main.cpp src/Image.cpp src/AsciiGenerator.cpp src/ImageExporter.cpp src/EdgeProcessor.cpp)

target_include_directories(AsciiArt PRIVATE external)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})