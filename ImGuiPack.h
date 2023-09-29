#pragma once
#pragma warning(disable : 4251)
#pragma warning(disable : 4005)

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"  // warning: unknown warning group 'xxx'                      // not all warnings are known by all Clang versions and they tend to be rename-happy.. so ignoring warnings triggers new
                                                             // warnings on some configuration. Great!
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"        // warning: unknown warning group 'xxx'
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning: use of old-style cast                            // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            // warning: comparing floating point with == or != is unsafe // storing and comparing against same constants (typically 0.0f) is ok.
#pragma clang diagnostic ignored "-Wformat-nonliteral"      // warning: format string is not a string literal            // passing non-literal to vsnformat(). yes, user passing incorrect format strings can crash the code.
#pragma clang diagnostic ignored "-Wexit-time-destructors"  // warning: declaration requires an exit-time destructor     // exit-time destruction order is undefined. if MemFree() leads to users code that has been disabled before exit it might cause
                                                            // problems. ImGui coding style welcomes static/globals.
#pragma clang diagnostic ignored "-Wglobal-constructors"    // warning: declaration requires a global destructor         // similar to above, not sure what the exact difference is.
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning: implicit conversion changes signedness
#pragma clang diagnostic ignored "-Wformat-pedantic"        // warning: format specifies type 'void *' but the argument has type 'xxxx *' // unreasonable, would lead to casting every %p arg to void*. probably enabled by -pedantic.
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"       // warning: cast to 'void *' from smaller integer type 'int'
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning: zero as null pointer constant                    // some standard header variations use #define NULL 0
#pragma clang diagnostic ignored "-Wdouble-promotion"               // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"  // warning: implicit conversion from 'xxx' to 'float' may lose precision
#elif defined(__GNUC__)
// We disable -Wpragmas because GCC doesn't provide a has_warning equivalent and some forks/patches may not follow the warning/version association.
#pragma GCC diagnostic ignored "-Wpragmas"              // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wunused-function"      // warning: 'xxxx' defined but not used
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"  // warning: cast to pointer from integer of different size
#pragma GCC diagnostic ignored "-Wformat"               // warning: format '%p' expects argument of type 'void*', but argument 6 has type 'ImGuiWindow*'
#pragma GCC diagnostic ignored "-Wdouble-promotion"     // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"           // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#pragma GCC diagnostic ignored "-Wformat-nonliteral"    // warning: format not a string literal, format string not checked
#pragma GCC diagnostic ignored "-Wstrict-overflow"      // warning: assuming signed overflow does not occur when assuming that (X - c) > X is always false
#pragma GCC diagnostic ignored "-Wclass-memaccess"      // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__WIN64__) || defined(WIN64) || defined(_WIN64) || defined(_MSC_VER)
#if defined(ImGuiPack_EXPORTS)
#define IMGUI_API __declspec(dllexport)
#define IMPLOT_API __declspec(dllexport)
#define IGFD_API __declspec(dllexport)
#define IMGUI_IMPL_API __declspec(dllexport)
#define IN_APP_GPU_PROFILER_API __declspec(dllexport)
#elif defined(IMGUI_PACK_SHARED_LIBS)
#define IMGUI_API __declspec(dllimport)
#define IMPLOT_API __declspec(dllimport)
#define IGFD_API __declspec(dllimport)
#define IMGUI_IMPL_API __declspec(dllimport)
#define IN_APP_GPU_PROFILER_API __declspec(dllimport)
#else
#define IMGUI_API
#define IMPLOT_API
#define IGFD_API
#define IMGUI_IMPL_API
#define IN_APP_GPU_PROFILER_API
#endif
#else
#define IMGUI_API
#define IMPLOT_API
#define IGFD_API
#define IMGUI_IMPL_API
#define IN_APP_GPU_PROFILER_API
#endif

#include <3rdparty/imgui/imgui.h>
#include <3rdparty/imgui_implot/implot.h>
#include <3rdparty/imgui/imgui_internal.h>
#include <3rdparty/imgui_tools/ImWidgets.h>
#include <3rdparty/imgui_tools/Messaging.h>
#include <3rdparty/imgui_imguizmo/ImGuizmo.h>
#include <3rdparty/imgui_tools/AbstractPane.h>
#include <3rdparty/imgui_imcoolbar/ImCoolbar.h>
#include <3rdparty/imgui_tools/LayoutManager.h>
#include <3rdparty/imgui_tools/ImGuiThemeHelper.h>
#include <3rdparty/imgui_markdown/imgui_markdown.h>
#include <3rdparty/inappgpuprofiler/InAppGpuProfiler.h>
#include <3rdparty/imgui_node_editor/imgui_node_editor.h>
#include <3rdparty/imgui_imguicolottextedit/TextEditor.h>
#include <3rdparty/imgui_imgradienthdr/src/ImGradientHDR.h>
#include <3rdparty/imgui_imguifiledialog/ImGuiFileDialog.h>
