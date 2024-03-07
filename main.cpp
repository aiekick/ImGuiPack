// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
Copyright 2022-2023 Stephane Cuillerdier (aka aiekick)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ImGuiPack.h>
#include <3rdparty/imgui_docking/backends/imgui_impl_opengl3.h>
#include <3rdparty/imgui_docking/backends/imgui_impl_glfw.h>
#include <stdio.h>

#include <src/MainFrame.h>

#define SHOW_CONSOLE

static void glfw_error_callback(int error, const char* description) { std::cout << "glfw error " << error << " : " << description << std::endl; }

int main(int, char** argv) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create window with graphics context
    GLFWwindow* mainWindow = glfwCreateWindow(1280, 720, "ImGuiPackDemoApp", nullptr, nullptr);
    if (mainWindow == 0) {
        std::cout << "Fail to create the window" << std::endl;
        return 1;
    }
    glfwMakeContextCurrent(mainWindow);
    glfwSwapInterval(1);  // Enable vsync

    if (gladLoadGL() == 0) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

#ifdef MSVC
#if defined(_DEBUG) && defined(SHOW_CONSOLE)
    ShowWindow(GetConsoleWindow(), SW_SHOW);  // show
#else
    ShowWindow(GetConsoleWindow(), SW_HIDE);  // hide
#endif
#endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Viewport
    io.FontAllowUserScaling = true;                      // activate zoom feature with ctrl + mousewheel
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
    io.ConfigViewportsNoDecoration = false;  // toujours mettre une frame au fenetre enfant
#endif

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // load memory font file
    /*auto fonts_ptr = ImGui::GetIO().Fonts;
    fonts_ptr->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_RM, 15.0f);
    static ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
    static const ImWchar icons_ranges[] = {ICON_MIN_NDP, ICON_MAX_NDP, 0};
    fonts_ptr->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_NDP, 15.0f, &icons_config, icons_ranges);
    static const ImWchar icons_ranges2[] = {ICON_MIN_NDP2, ICON_MAX_NDP2, 0};
    fonts_ptr->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_NDP2, 15.0f, &icons_config, icons_ranges2);
    static const ImWchar icons_ranges3[] = {ICON_MIN_NDPTB, ICON_MAX_NDPTB, 0};
    fonts_ptr->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_NDPTB, 15.0f, &icons_config, icons_ranges3);
*/
    auto* mainFramePtr = MainFrame::Instance();
    
    mainFramePtr->init();

    // Main loop
    int display_w, display_h;
    ImVec2 pos, size;
    while (!glfwWindowShouldClose(mainWindow)) {
        glfwPollEvents();

        glfwGetFramebufferSize(mainWindow, &display_w, &display_h);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            if (viewport) {
                pos = viewport->WorkPos;
                size = viewport->WorkSize;
            }
        } else {
            pos = ImVec2(0, 0);
            size = ImVec2((float)display_w, (float)display_h);
        }

        mainFramePtr->display(pos, size);

        ImGui::Render();

        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        // For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(mainWindow);
    }

    mainFramePtr->unit();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();

    glfwDestroyWindow(mainWindow);
    glfwTerminate();

    return 0;
}
