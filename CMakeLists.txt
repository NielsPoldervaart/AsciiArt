cmake_minimum_required(VERSION 3.30)
project(AsciiArt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_UPDATER "Enable auto-updater" OFF)
set(APP_VERSION "" CACHE STRING "")
set(GITHUB_REPO_OWNER "" CACHE STRING "")

add_executable(AsciiArt src/main.cpp
        src/Image.cpp
        src/AsciiGenerator.cpp
        src/ImageExporter.cpp
        src/EdgeProcessor.cpp
        src/AsciiExporter.cpp
        src/UpdateChecker.cpp
        src/AppConfig.cpp
)

target_include_directories(AsciiArt PRIVATE include external)

if (ENABLE_UPDATER)
    target_compile_definitions(AsciiArt PRIVATE
            ENABLE_UPDATER=true
            APP_VERSION=\"${APP_VERSION}\"
            REPO_OWNER=\"${GITHUB_REPO_OWNER}\"
            REPO_NAME=\"${CMAKE_PROJECT_NAME}\"
    )
else ()
    target_compile_definitions(AsciiArt PRIVATE
            ENABLE_UPDATER=false
            APP_VERSION=\"\"
            REPO_OWNER=\"\"
            REPO_NAME=\"\"
    )
endif ()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})