#pragma once
#include <GLFW/glfw3.h>

namespace WindowBackend
{
    GLFWwindow* InitGraphics(int width, int height, const char* title);
    void BeginFrame();
    void EndFrame(GLFWwindow* window);
    void Shutdown();
}