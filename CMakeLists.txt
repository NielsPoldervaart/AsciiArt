cmake_minimum_required(VERSION 3.30)
project(AsciiArt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(APP_VERSION "vLocal-Dev" CACHE STRING "The app version")

add_executable(AsciiArt src/main.cpp
        src/Image.cpp
        src/AsciiGenerator.cpp
        src/ImageExporter.cpp
        src/EdgeProcessor.cpp
        src/AsciiExporter.cpp
        src/UpdateChecker.cpp
)

target_include_directories(AsciiArt PRIVATE external)

target_compile_definitions(AsciiArt PRIVATE APP_VERSION=\"${APP_VERSION}\")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})