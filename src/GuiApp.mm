#include "GuiApp.h"
#include "WindowBackend.h"
#include "imgui.h"
#include "portable-file-dialogs.h"

namespace
{
    std::string currentImagePath;

    void OnFileDropped(const std::string& path)
    {
        currentImagePath = path;
    }
}

int RunGuiApp()
{
    if (!WindowBackend::Init(1280, 720, "AsciiArt v3.0")) return 1;

    WindowBackend::SetFileDropCallback(OnFileDropped);

    float clearColor[4] = { 0.075f, 0.075f, 0.075f, 1.00f };

    while (WindowBackend::IsRunning())
    {
        WindowBackend::ProcessEvents();

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
            auto selection = pfd::open_file("Select an image", ".",
                { "Image Files", "*.png *.jpg *.jpeg *.bmp" }).result();

            if (!selection.empty())
                currentImagePath = selection[0];
        }

        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button("Quit Application"))
        {
            WindowBackend::Close();
        }
        ImGui::PopStyleColor();

        ImGui::End();

        ImGui::Begin("Image Preview");
        if (!currentImagePath.empty())
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Loaded: %s", currentImagePath.c_str());
        else
            ImGui::Text("Drag and drop your image here, or click 'Load Image'");
        ImGui::End();

        WindowBackend::EndFrame(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

#ifdef __APPLE__
        }
#endif
    }

    WindowBackend::Shutdown();
    return 0;
}