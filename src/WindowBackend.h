#pragma once
#include <string>
#include <functional>

namespace WindowBackend
{
    bool Init(int width, int height, const char* title);
    bool IsRunning();
    void ProcessEvents();
    void Close();

    void BeginFrame();
    void EndFrame(float clearR, float clearG, float clearB, float clearA);
    void Shutdown();

    void SetFileDropCallback(std::function<void(const std::string&)> callback);
}