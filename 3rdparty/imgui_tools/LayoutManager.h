/*
Copyright 2022-2024 Stephane Cuillerdier (aka aiekick)

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

#include <ImGuiPack.h>

#ifdef USE_XML_CONFIG
#include <ctools/ConfigAbstract.h>
#endif  // USE_XML_CONFIG

#ifndef ILAYOUT_PANE_INCLUDE
#include <ILayoutPane.h>
#else
#include ILAYOUT_PANE_INCLUDE
#endif

#include <3rdparty/imgui_docking/imgui.h>
#include <array>
#include <string>
#include <unordered_map>
#include <map>

#include <memory>  // smart ptr
#include <string>
#include <cstdint>

struct ImGuiContext;
struct ImVec2;
struct ImRect;

typedef std::string PaneDisposal;
// can be :
// CENTRAL
// LEFT
// RIGHT
// TOP
// BOTTOM

typedef std::string PaneCategoryName;

class AbstractPane : public ILayoutPane {
public:
    bool Init() override = 0;  // return false if the init was failed
    void Unit() override = 0;

    // the return, is a user side use case here
    bool DrawPanes(const uint32_t& vCurrentFrame, bool* vOpened, ImGuiContext* vContextPtr, void* vUserDatas) override = 0;
    bool DrawWidgets(const uint32_t& /*vCurrentFrame*/, ImGuiContext* /*vContextPtr*/, void* /*vUserDatas*/) override {
        return false;
    }
    bool DrawOverlays(const uint32_t& /*vCurrentFrame*/, const ImRect& /*vRect*/, ImGuiContext* /*vContextPtr*/, void* /*vUserDatas*/) override {
        return false;
    }
    bool DrawDialogsAndPopups(const uint32_t& /*vCurrentFrame*/, const ImRect& /*vRect*/, ImGuiContext* /*vContextPtr*/, void* /*vUserDatas*/) override {
        return false;
    }

    // if for any reason the pane must be hidden temporary, the user can control this here
    bool CanBeDisplayed() override {
        return true;
    }
};

typedef std::shared_ptr<AbstractPane> AbstractPanePtr;
typedef std::weak_ptr<AbstractPane> AbstractPaneWeak;

class ProjectFile;
class IMGUI_API LayoutManager
#ifdef USE_XML_CONFIG
    : public conf::ConfigAbstract
#endif  // USE_XML_CONFIG
{
private:
    class InternalPane {
    public:
        ILayoutPaneWeak ilayoutPane;
        LayoutPaneName paneName;
        LayoutPaneFlag paneFlag = 0;
        PaneDisposal paneDisposal = "CENTRAL";
        float paneDisposalRatio = 0.5f;
        bool openedDefault = false;
        bool focusedDefault = false;
        bool showPaneAtFirstCall = false;
        bool hidePaneAtFirstCall = false;
        int32_t paneWidgetId = 0;
        PaneCategoryName paneCategory;
    };
    typedef std::shared_ptr<InternalPane> InternalPanePtr;
    typedef std::weak_ptr<InternalPane> InternalPaneWeak;

private:
    ImGuiID m_DockSpaceID = 0;
    bool m_FirstLayout = false;
    bool m_FirstStart = true;
    std::string m_MenuLabel;
    std::string m_DefaultMenuLabel;

protected:
    int32_t m_FlagCount = 0U;
    std::map<LayoutPaneName, InternalPanePtr> m_PanesByName;  // InternalPanePtr are saved here
    std::map<LayoutPaneFlag, InternalPaneWeak> m_PanesByFlag;
    std::map<PaneDisposal, std::vector<InternalPaneWeak>> m_PanesByDisposal;
    std::map<PaneCategoryName, std::vector<InternalPaneWeak>> m_PanesInDisplayOrder;
    std::unordered_map<PaneDisposal, float> m_PanesDisposalRatios;

public:
    LayoutPaneFlag m_Pane_Focused_Default = 0;
    LayoutPaneFlag m_Pane_Opened_Default = 0;
    LayoutPaneFlag pane_Shown = 0;
    LayoutPaneFlag m_Pane_Focused = 0;
    LayoutPaneFlag m_Pane_Hovered = 0;
    LayoutPaneFlag m_Pane_LastHovered = 0;
    ImVec2 m_LastSize;

public:
    bool AddPane(ILayoutPaneWeak vPane,
                 const LayoutPaneName& vName,
                 const PaneCategoryName& vCategory,
                 const PaneDisposal& vPaneDisposal,
                 const float& vPaneDisposalRatio,
                 const bool& vIsOpenedDefault,
                 const bool& vIsFocusedDefault);
    void RemovePane(const LayoutPaneName& vName);
    void SetPaneDisposalRatio(const PaneDisposal& vPaneDisposal, const float& vRatio);

public:
    void Init(const std::string& vMenuLabel, const std::string& vDefaultMenuLabel, const bool& vForceDefaultLayout = false);
    void Unit();

    bool InitPanes();
    void UnitPanes();

    void InitAfterFirstDisplay(const ImVec2& vSize);
    bool BeginDockSpace(const ImGuiDockNodeFlags& vFlags, const ImVec2& voffset = ImVec2());
    void EndDockSpace();
    bool IsDockSpaceHoleHovered();

    void ApplyInitialDockingLayout(const ImVec2& vSize = ImVec2(0, 0));

    virtual void DisplayMenu(const ImVec2& vSize);
    virtual bool DrawWidgets(const uint32_t& vCurrentFrame, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr);
    virtual bool DrawOverlays(const uint32_t& vCurrentFrame, const ImRect& vRect, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr);
    virtual bool DrawPanes(const uint32_t& vCurrentFrame, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr);
    virtual bool DrawDialogsAndPopups(const uint32_t& vCurrentFrame, const ImRect& vRect, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr);

public:
    void ShowSpecificPane(const LayoutPaneFlag& vPane);
    void HideSpecificPane(const LayoutPaneFlag& vPane);
    void FocusSpecificPane(const LayoutPaneFlag& vPane);
    void FocusSpecificPane(const std::string& vlabel);
    void ShowAndFocusSpecificPane(const LayoutPaneFlag& vPane);
    bool IsSpecificPaneFocused(const LayoutPaneFlag& vPane);
    bool IsSpecificPaneFocused(const std::string& vlabel);
    void AddSpecificPaneToExisting(const std::string& vNewPane, const std::string& vExistingPane);

private:  // configuration
    LayoutPaneFlag m_GetFocusedPanes();
    void m_SetFocusedPanes(const LayoutPaneFlag& vActivePanes);
    std::vector<std::string> m_ParsePaneDisposal(const PaneDisposal& vPaneDisposal);

#ifdef USE_XML_CONFIG
public:  // configuration
    std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
    bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;
#endif  // USE_XML_CONFIG

public:  // singleton
    static LayoutManager* Instance(LayoutManager* vCopy = nullptr, bool vForce = false) {
        static LayoutManager _instance;
        static LayoutManager* _instance_copy = nullptr;
        if (vCopy || vForce) {
            _instance_copy = vCopy;
        }
        if (_instance_copy) {
            return _instance_copy;
        }
        return &_instance;
    }

protected:
    LayoutManager();  // Prevent construction
    LayoutManager(const LayoutManager&) = delete;
    LayoutManager& operator=(const LayoutManager&) = delete;
    virtual ~LayoutManager();  // Prevent unwanted destruction
};
