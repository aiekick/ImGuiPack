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