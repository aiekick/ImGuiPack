// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <ImGuiPack.h>
#include <3rdparty/glad/include/glad/glad.h>
#include <3rdparty/imgui_docking_layout/backends/imgui_impl_opengl3.h>
#include <3rdparty/imgui_docking_layout/backends/imgui_impl_glfw.h>
#include <CustomFont.cpp>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <clocale>
#include <string>
#include <vector>
#include <array>

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

int display_w, display_h;

static void glfw_error_callback(int error, const char* description) { fprintf(stderr, "Glfw Error %d: %s\n", error, description); }

int main(int, char**) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImToolbar", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform Windows
    io.FontAllowUserScaling = true;  // zoom wiht ctrl + mouse wheel

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // load icon font file (CustomFont.cpp)
    ImGui::GetIO().Fonts->AddFontDefault();
    static const ImWchar icons_ranges[] = {ICON_MIN_IGFD, ICON_MAX_IGFD, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode  = true;
    icons_config.PixelSnapH = true;
    ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_IGFD, 50.0f, &icons_config, icons_ranges);

    const std::vector<std::string> icons_name = {
        "Settings",   //
        "Activa",     //
        "Magnet",     //
        "Blender",    //
        "CLion",      //
        "Firefox",    //
        "Gimp",       //
        "Godot",      //
        "VLC",        //
        "Wikipedia",  //
        "GeoGebra"    //
    };

    auto background_id = loadTexture("res/desert.jpg");

    AppDatas _appDatas;
    for (const auto& name : icons_name) {
        _appDatas.textures.push_back(std::make_pair(name, loadTexture("res/" + name + ".png")));
    }

    const char* imCoolBarTitle = "##CoolBarMainWin";

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glfwGetFramebufferSize(window, &display_w, &display_h);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        drawBackground(background_id);

        drawCoolBar(_appDatas, 11, "Top##CoolBarMainWin", ImCoolBarFlags_Horizontal, {ImVec2(0.5f, 0.0f), 50.0f, 100.0f});
        drawCoolBar(_appDatas, 6, "Left##CoolBarMainWin", ImCoolBarFlags_Vertical, {ImVec2(0.0f, 0.5f), 50.0f, 100.0f});
        drawCoolBar(_appDatas, 6, "Right##CoolBarMainWin", ImCoolBarFlags_Vertical, {ImVec2(1.0f, 0.5f), 50.0f, 100.0f});

        const float& ref_font_scale = ImGui::GetIO().Fonts->Fonts[0]->Scale;

        auto coolbar_button = [ref_font_scale](const char* label) {
            float w         = ImGui::GetCoolBarItemWidth();
            auto font_ptr   = ImGui::GetIO().Fonts->Fonts[0];
            //font_ptr->Scale = ref_font_scale;
            ImGui::PushFont(font_ptr);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2());
            ImGui::PopStyleVar();
            font_ptr->Scale = ImGui::GetCoolBarItemScale();
            ImGui::Button(label, ImVec2(w, w));
            font_ptr->Scale = ref_font_scale;
            ImGui::PopFont();
        };

        static ImGui::ImCoolBarConfig _config;
        _config.normal_size  = 25.0f;
        _config.hovered_size = 100.0f;
        _config.anchor       = ImVec2(0.5f, 1.0f);

        ImGui::GetIO().Fonts->Fonts[0]->Scale = ref_font_scale;
        ImGuiWindowFlags window_flags         = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings ;
        if (ImGui::BeginViewportSideBar("BottomBar", ImGui::GetMainViewport(), ImGuiDir_Down, 40.0f, window_flags)) {
            if (ImGui::BeginCoolBar("Bottom##CoolBarMainWin", ImCoolBarFlags_Horizontal, _config)) {
                auto window = ImGui::GetCurrentWindow();
                if (window) {
                    // correct the rect of the window. maybe a bug on imgui !?
                    // the workrect can cause issue when click around
                    // this thing correct the issue
                    const auto& rc            = window->Rect();
                    window->WorkRect          = rc;
                    window->OuterRectClipped  = rc;
                    window->InnerRect         = rc;
                    window->InnerClipRect     = rc;
                    window->ParentWorkRect    = rc;
                    window->ClipRect          = rc;
                    window->ContentRegionRect = rc;
                }
                if (ImGui::CoolBarItem()) {
                    coolbar_button("A");
                }
                if (ImGui::CoolBarItem()) {
                    coolbar_button("B");
                }
                if (ImGui::CoolBarItem()) {
                    coolbar_button("C");
                }
                if (ImGui::CoolBarItem()) {
                    coolbar_button("D");
                }
                if (ImGui::CoolBarItem()) {
                    coolbar_button("E");
                }
                if (ImGui::CoolBarItem()) {
                    coolbar_button("F");
                }
                if (ImGui::CoolBarItem()) {
                    coolbar_button("G");
                }
                if (ImGui::CoolBarItem()) {
                    coolbar_button("H");
                }
                if (ImGui::CoolBarItem()) {
                    coolbar_button("I");
                }
                if (ImGui::CoolBarItem()) {
                    coolbar_button("J");
                }
                if (ImGui::CoolBarItem()) {
                    coolbar_button("K");
                }
                if (ImGui::CoolBarItem()) {
                    coolbar_button("L");
                }
                if (ImGui::CoolBarItem()) {
                    coolbar_button("M");
                }
                ImGui::EndCoolBar();
            }
        }
        ImGui::End();

        if (_appDatas.show_app_metrics) {
            ImGui::ShowMetricsWindow(&_appDatas.show_app_metrics);
        }

        if (_appDatas.show_app_demo) {
            ImGui::ShowDemoWindow(&_appDatas.show_app_demo);
        }

        if (_appDatas.show_graph_demo) {
            ImPlot::ShowDemoWindow(&_appDatas.show_graph_demo);
        }

        // Cpu Zone : prepare
        ImGui::Render();

        // GPU Zone : Rendering
        glfwMakeContextCurrent(window);

        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
