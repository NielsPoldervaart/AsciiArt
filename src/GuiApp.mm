#include "GuiApp.h"
#include "WindowBackend.h"
#include "imgui.h"

int RunGuiApp()
{
    if (!glfwInit()) return 1;

    GLFWwindow* window = WindowBackend::InitGraphics(1280, 720, "AsciiArt v3.0");
    if (!window)
    {
        glfwTerminate();
        return 1;
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

#ifdef __APPLE__
        @autoreleasepool {
#endif

        WindowBackend::BeginFrame();

        // TODO: add actual GUI stuff

        ImGui::ShowDemoWindow();

        WindowBackend::EndFrame(window);

#ifdef __APPLE__
        }
#endif
    }

    WindowBackend::Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}