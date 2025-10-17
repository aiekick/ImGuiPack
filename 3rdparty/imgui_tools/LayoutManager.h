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
#pragma warning(disable : 4275)

#include <imguipack.h>

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

typedef std::string PaneMenuCategoryName;
typedef std::string PaneMenuName;

class IMGUI_API AbstractPane : public ILayoutPane {
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
#ifdef EZ_TOOLS_XML_CONFIG
    : public ez::xml::Config
#endif  // EZ_TOOLS_XML_CONFIG
{
public:
    class PaneInfos {
        friend class LayoutManager;

    private:
        LayoutPaneFlag m_paneFlag{0};
        bool m_showPaneAtFirstCall{false};
        bool m_hidePaneAtFirstCall{false};
        int32_t m_paneWidgetId{0}; // tofix : seem unused

    public:
        ILayoutPaneWeak ilayoutPane;
        LayoutPaneName paneName;
        PaneDisposal paneDisposal{"CENTRAL"};
        float paneDisposalRatio{0.5f};
        bool openedDefault{false};
        bool focusedDefault{false};
        PaneMenuCategoryName paneMenuCategoryName;
        PaneMenuName paneMenuName;
        PaneInfos() = default;
        PaneInfos(
            ILayoutPaneWeak vPane,
            const LayoutPaneName& vName,
            const PaneMenuCategoryName& vMenuCategory,
            const PaneMenuName& vMenuName,
            const PaneDisposal& vPaneDisposal,
            const float vPaneDisposalRatio,
            const bool vIsOpenedDefault,
            const bool vIsFocusedDefault);
        PaneInfos(ILayoutPaneWeak vPane, const std::string& vName);
        PaneInfos& setMenu(const std::string& vName, const std::string& vCategory = {});
        PaneInfos& setDisposalSide(const std::string& vDisposal, const float vDisposalRatio = 0.4f);
        PaneInfos& setDisposalCentral();
        PaneInfos& setDefaultOpened(const bool vOpened);
        PaneInfos& setDefaultFocused(const bool vFocused);

    private:
        void m_finalize();
    };

private:
    typedef std::shared_ptr<PaneInfos> InternalPanePtr;
    typedef std::weak_ptr<PaneInfos> InternalPaneWeak;

    ImGuiID m_DockSpaceID{0};
    bool m_FirstLayout{false};
    bool m_FirstStart{true};
    std::string m_MenuLabel;
    std::string m_DefaultMenuLabel;

    LayoutPaneFlag m_paneFocusedDefault{0};
    LayoutPaneFlag m_paneOpenedDefault{0};
    LayoutPaneFlag m_paneShown{0};
    LayoutPaneFlag m_paneFocused{0};
    LayoutPaneFlag m_paneHovered{0};
    LayoutPaneFlag m_paneLastHovered{0};
    ImVec2 m_lastViewportSize;

protected:
    int32_t m_FlagCount{0};
    std::vector<InternalPaneWeak> m_OrderesPanes;
    std::map<LayoutPaneName, InternalPanePtr> m_PanesByName;  // InternalPanePtr are saved here
    std::map<LayoutPaneFlag, InternalPaneWeak> m_PanesByFlag;
    std::map<PaneDisposal, std::vector<InternalPaneWeak>> m_PanesByDisposal;
    std::map<PaneMenuCategoryName, std::vector<InternalPaneWeak>> m_PanesInDisplayOrder;
    std::map<PaneDisposal, float> m_PanesDisposalRatios;

public:
    bool AddPane(const PaneInfos& vPaneInfos);
    void RemovePane(const LayoutPaneName& vName);
    void SetPaneDisposalRatio(const PaneDisposal& vPaneDisposal, const float vRatio);

    void Init(const std::string& vMenuLabel, const std::string& vDefaultMenuLabel, const bool vForceDefaultLayout = false);
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

    void ShowSpecificPane(const LayoutPaneFlag vPane);
    void HideSpecificPane(const LayoutPaneFlag vPane);
    void FocusSpecificPane(const LayoutPaneFlag vPane);
    void FocusSpecificPane(const std::string& vlabel);
    void ShowAndFocusSpecificPane(const LayoutPaneFlag vPane);
    bool IsSpecificPaneFocused(const LayoutPaneFlag vPane);
    bool IsSpecificPaneFocused(const std::string& vlabel);
    void AddSpecificPaneToExisting(const std::string& vNewPane, const std::string& vExistingPane);

    const LayoutPaneFlag& getPaneFocusedDefault() const { return m_paneFocusedDefault; }
    const LayoutPaneFlag& getPaneOpenedDefault() const { return m_paneOpenedDefault; }
    const LayoutPaneFlag& getPaneShown() const { return m_paneShown; }
    const LayoutPaneFlag& getPaneFocused() const { return m_paneFocused; }
    const LayoutPaneFlag& getPaneHovered() const { return m_paneHovered; }
    const LayoutPaneFlag& getPaneLastHovered() const { return m_paneLastHovered; }
    const ImVec2& getLastViewportSize() const { return m_lastViewportSize; }

private:  // configuration
    LayoutPaneFlag m_GetFocusedPanes();
    void m_SetFocusedPanes(const LayoutPaneFlag vActivePanes);
    std::vector<std::string> m_ParsePaneDisposal(const PaneDisposal& vPaneDisposal);

#ifdef EZ_TOOLS_XML_CONFIG
public:  // configuration
    ez::xml::Nodes getXmlNodes(const std::string& vUserDatas = "") override;
    bool setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) override;
#endif  // EZ_TOOLS_XML_CONFIG

public:  // singleton
// old behavior
#ifdef LEGACY_SINGLETON
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
#else   // LEGACY_SINGLETON
    static std::unique_ptr<LayoutManager>& initSingleton() {
        static auto mp_instance = std::unique_ptr<LayoutManager>(new LayoutManager());
        return mp_instance;
    }
    static LayoutManager& ref() { return *initSingleton().get(); }
    static void unitSingleton() { initSingleton().reset(); }
#endif  // LEGACY_SINGLETON

public:
    LayoutManager();  // Prevent construction
    LayoutManager(const LayoutManager&) = delete;
    LayoutManager& operator=(const LayoutManager&) = delete;
    virtual ~LayoutManager();  // Prevent unwanted destruction
};

typedef LayoutManager::PaneInfos LayoutPaneInfos;
