cmake_minimum_required(VERSION 3.30)
project(AsciiArt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_UPDATER "Enable auto-updater" OFF)
set(APP_VERSION "" CACHE STRING "")
set(GITHUB_REPO_OWNER "" CACHE STRING "")

include(FetchContent)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)

add_executable(AsciiArt src/main.cpp
        src/Image.cpp
        src/AsciiGenerator.cpp
        src/ImageExporter.cpp
        src/EdgeProcessor.cpp
        src/AsciiExporter.cpp
        src/UpdateChecker.cpp
        src/AppConfig.cpp

        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/imgui_demo.cpp

        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(AsciiArt PRIVATE
        external
        external/imgui
        external/imgui/backends
)

target_link_libraries(AsciiArt PRIVATE glfw OpenGL::GL)

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