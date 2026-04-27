#include "GuiApp.h"
#include "WindowBackend.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>

int RunGuiApp()
{
    if (!glfwInit()) return 1;

    GLFWwindow* window = WindowBackend::InitGraphics(1280, 720, "AsciiArt v3.0");
    if (!window)
    {
        glfwTerminate();
        return 1;
    }

    float clearColor[4] = { 0.075f, 0.075f, 0.075f, 1.00f };

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

#ifdef __APPLE__
        @autoreleasepool {
#endif

        WindowBackend::BeginFrame();

        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        ImGui::Begin("Settings");
        ImGui::Text("ASCII Generation Settings");
        ImGui::Separator();

        if (ImGui::Button("Load Image"))
        {
            // TODO: Add actuak load image logic
        }

        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button("Quit Application"))
        {
            glfwSetWindowShouldClose(window, true);
        }
        ImGui::PopStyleColor();

        ImGui::End();

        ImGui::Begin("Image Preview");
        ImGui::Text("Your amazing ASCII art :)");
        ImGui::End();

        WindowBackend::EndFrame(window, clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

#ifdef __APPLE__
        }
#endif
    }

    WindowBackend::Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}