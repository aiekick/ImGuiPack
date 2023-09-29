/*
Copyright 2022-2022 Stephane Cuillerdier (aka aiekick)

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

#pragma once
#pragma warning(disable : 4005)
#pragma warning(disable : 4100)
#pragma warning(disable : 4251)

#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__WIN64__) || defined(WIN64) || defined(_WIN64) || defined(_MSC_VER)
#if defined(ImGuiPack_EXPORTS)
#define IMGUI_API __declspec(dllexport)
#define IMPLOT_API __declspec(dllexport)
#define IGFD_API __declspec(dllexport)
#define IMGUI_IMPL_API __declspec(dllexport)
#elif defined(IMGUI_PACK_SHARED_LIBS)
#define IMGUI_API __declspec(dllimport)
#define IMPLOT_API __declspec(dllimport)
#define IGFD_API __declspec(dllimport)
#define IMGUI_IMPL_API __declspec(dllimport)
#else
#define IMGUI_API
#define IMPLOT_API
#define IGFD_API
#define IMGUI_IMPL_API
#endif
#else
#define IMGUI_API
#define IMPLOT_API
#define IGFD_API
#define IMGUI_IMPL_API
#endif

#include <memory>  // smart ptr
#include <string>
#include <cstdint>

#include <imgui.h>
#include <imgui_internal.h>

typedef int PaneFlags;
enum class PaneDisposal { CENTRAL = 0, LEFT, RIGHT, BOTTOM, TOP, Count };

typedef std::string PaneCategoryName;

class AbstractPane;
typedef std::shared_ptr<AbstractPane> AbstractPanePtr;
typedef std::weak_ptr<AbstractPane> AbstractPaneWeak;

class BaseNode;
class ProjectFile;
class IMGUI_API AbstractPane {
public:
    std::string paneName;
    PaneFlags paneFlag = 0;
    PaneDisposal paneDisposal = PaneDisposal::CENTRAL;
    bool openedDefault = false;
    bool focusedDefault = false;
    bool showPaneAtFirstCall = false;
    bool hidePaneAtFirstCall = false;
    int32_t paneWidgetId = 0;
    PaneCategoryName paneCategory;

public:
    int32_t NewWidgetId() {
        return ++paneWidgetId;
    }
    int32_t GetWidgetId() {
        return paneWidgetId;
    }

public:
    virtual bool Init() = 0;
    virtual void Unit() = 0;

    virtual bool DrawPanes(const uint32_t& vCurrentFrame, PaneFlags& vInOutPaneShown, ImGuiContext* vContextPtr = nullptr,
                           const std::string& vUserDatas = {}) = 0;
    virtual bool DrawWidgets(const uint32_t& vCurrentFrame, ImGuiContext* vContextPtr = nullptr, const std::string& vUserDatas = {}) {
        return false;
    }
    virtual bool DrawOverlays(const uint32_t& vCurrentFrame, const ImRect& vRect, ImGuiContext* vContextPtr = nullptr,
                              const std::string& vUserDatas = {}) {
        return false;
    }
    virtual bool DrawDialogsAndPopups(const uint32_t& vCurrentFrame, const ImVec2& vMaxSize, ImGuiContext* vContextPtr = nullptr,
                                      const std::string& vUserDatas = {}) {
        return false;
    }

public:
    virtual void ShowPane() {
        showPaneAtFirstCall = true;
    };
    virtual void HidePane() {
        hidePaneAtFirstCall = true;
    };
    virtual bool CanWeDisplay() {
        return true;
    };
};