#include "WindowBackend.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"

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
    MTLClearColor clearColor;
#endif

    GLFWwindow* InitGraphics(int width, int height, const char* title)
    {
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
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsDark();

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

        clearColor = MTLClearColorMake(0.075, 0.075, 0.075, 1.0);
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
#ifdef __APPLE__
        // Metal frame preparation is handled in EndFrame due to autoreleasepool scoping
#else
        ImGui_ImplOpenGL3_NewFrame();
#endif
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void EndFrame(GLFWwindow* window)
    {
        ImGui::Render();

#ifdef __APPLE__
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        metalLayer.drawableSize = CGSizeMake(width, height);
        id<CAMetalDrawable> drawable = [metalLayer nextDrawable];

        id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
        MTLRenderPassDescriptor* renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
        renderPassDescriptor.colorAttachments[0].clearColor = clearColor;
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
        glClearColor(0.075f, 0.075f, 0.075f, 1.0f);
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