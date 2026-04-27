#include "WindowBackend.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <iostream>

#ifdef __APPLE__
    #import <Metal/Metal.h>
    #import <QuartzCore/QuartzCore.h>
    #define GLFW_EXPOSE_NATIVE_COCOA
    #include <GLFW/glfw3native.h>
    #include "imgui_impl_metal.h"
#else
    #include "imgui_impl_opengl3.h"
#endif

namespace WindowBackend
{
#ifdef __APPLE__
    id<MTLCommandQueue> commandQueue;
    CAMetalLayer* metalLayer;
#endif

    static void glfw_error_callback(int error, const char* description)
    {
        std::cerr << "GLFW Error " << error << ": " << description << "\n";
    }

    GLFWwindow* InitGraphics(int width, int height, const char* title)
    {
        glfwSetErrorCallback(glfw_error_callback);

#ifdef __APPLE__
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

        GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window) return nullptr;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();

        float main_scale = 1.0f;
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        if (monitor) {
            main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(monitor);
        }
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);

        ImFont* font = io.Fonts->AddFontFromFileTTF("fonts/VT323.ttf", 18.0f * main_scale);
        if (font == nullptr) {
            std::cerr << "Warning: Could not load VT323.ttf. Falling back to default font.\n";
        }

#ifdef __APPLE__
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        commandQueue = [device newCommandQueue];

        ImGui_ImplGlfw_InitForOther(window, true);
        ImGui_ImplMetal_Init(device);

        NSWindow* nswin = glfwGetCocoaWindow(window);
        metalLayer = [CAMetalLayer layer];
        metalLayer.device = device;
        metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        nswin.contentView.layer = metalLayer;
        nswin.contentView.wantsLayer = YES;
#else
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");
#endif

        return window;
    }

    void BeginFrame()
    {
#ifndef __APPLE__
        ImGui_ImplOpenGL3_NewFrame();
#endif
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void EndFrame(GLFWwindow* window, float clearR, float clearG, float clearB, float clearA)
    {
        ImGui::Render();

#ifdef __APPLE__
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        metalLayer.drawableSize = CGSizeMake(width, height);
        id<CAMetalDrawable> drawable = [metalLayer nextDrawable];

        id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
        MTLRenderPassDescriptor* renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];

        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearR, clearG, clearB, clearA);
        renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
        renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;

        id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        ImGui_ImplMetal_NewFrame(renderPassDescriptor);
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);

        [renderEncoder endEncoding];
        [commandBuffer presentDrawable:drawable];
        [commandBuffer commit];
#else
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearR, clearG, clearB, clearA);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
#endif
    }

    void Shutdown()
    {
#ifdef __APPLE__
        ImGui_ImplMetal_Shutdown();
#else
        ImGui_ImplOpenGL3_Shutdown();
#endif
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}