#pragma once
#pragma warning(disable : 4251)

#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__WIN64__) || defined(WIN64) || defined(_WIN64) || defined(_MSC_VER)
#if defined(ImGuiPack_EXPORTS)
#define IMGUI_API __declspec(dllexport)
#define IMPLOT_API __declspec(dllexport)
#elif defined(BUILD_SHARED_LIBS)
#define IMGUI_API __declspec(dllimport)
#define IMPLOT_API __declspec(dllimport)
#else
#define IMGUI_API
#define IMPLOT_API
#endif
#else
#define IMGUI_API
#define IMPLOT_API
#endif

#include <3rdparty/imgui/imgui.h>
#include <3rdparty/imgui_imcoolbar/ImCoolbar.h>
#include <3rdparty/imgui_imgradienthdr/src/ImGradientHDR.h>
#include <3rdparty/imgui_imguifiledialog/ImGuiFileDialog.h>
#include <3rdparty/imgui_imguizmo/ImGuizmo.h>
#include <3rdparty/imgui_implot/implot.h>
#include <3rdparty/imgui_node_editor/NodeEditor/Include/imgui_node_editor.h>
#include <3rdparty/imgui_tools/AbstractPane.h>
#include <3rdparty/imgui_tools/ImWidgets.h>
#include <3rdparty/imgui_tools/LayoutManager.h>
#include <3rdparty/imgui_tools/Messaging.h>
#include <3rdparty/imgui_tools/ImGuiThemeHelper.h>
