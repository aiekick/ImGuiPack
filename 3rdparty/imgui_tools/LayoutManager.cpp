// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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

#include "LayoutManager.h"
#include <imguipack.h>
#include <cstdarg>  // For va_start, etc.
#include <set>

#include <imgui_internal.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///// LayoutManagerException ////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

class LayoutManagerException : public std::exception {
private:
    std::string m_msg;

public:
    LayoutManagerException() : std::exception() {}
    // std::exception(msg) is not availaiable on linux it seems... but on windows yes (cpp11)
    explicit LayoutManagerException(const std::string& vMsg) : std::exception(), m_msg(vMsg) {}
    explicit LayoutManagerException(char const* const fmt, ...) : std::exception() {
        va_list args;
        va_start(args, fmt);
        char TempBuffer[1024 + 1];
        const int w = vsnprintf(TempBuffer, 1024 + 1, fmt, args);
        va_end(args);
        if (w) {
            m_msg = std::string(TempBuffer, (size_t)w);
        }
    }
    ~LayoutManagerException() override = default;
    char const* what() const noexcept override { return m_msg.c_str(); }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///// LayoutManager::PaneInfos //////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LayoutManager::PaneInfos::PaneInfos(
    ILayoutPaneWeak vPane,
    const LayoutPaneName& vName,
    const PaneMenuCategoryName& vMenuCategory,
    const PaneMenuName& vMenuName,
    const PaneDisposal& vPaneDisposal,
    const float vPaneDisposalRatio,
    const bool vIsOpenedDefault,
    const bool vIsFocusedDefault)
    : ilayoutPane(vPane),
      paneName(vName),
      paneMenuCategoryName(vMenuCategory),
      paneMenuName(!vMenuName.empty() ? vMenuName : vName),
      paneDisposal(vPaneDisposal),
      paneDisposalRatio(vPaneDisposalRatio),
      openedDefault(vIsOpenedDefault),
      focusedDefault(vIsFocusedDefault) {}

LayoutManager::PaneInfos::PaneInfos(ILayoutPaneWeak vPane, const std::string& vName) : ilayoutPane(vPane), paneName(vName) {}
LayoutManager::PaneInfos& LayoutManager::PaneInfos::setMenu(const std::string& vName, const std::string& vCategory){
    paneMenuName = vName;
    paneMenuCategoryName = vCategory;
    return *this;
}
LayoutManager::PaneInfos& LayoutManager::PaneInfos::setDisposalSide(const std::string& vDisposal, const float vDisposalRatio) {
    paneDisposal = vDisposal;
    paneDisposalRatio = vDisposalRatio;
    return *this;
}
LayoutManager::PaneInfos& LayoutManager::PaneInfos::setDisposalCentral() {
    setDisposalSide("CENTRAL", 0.0f);
    return *this;
}
LayoutManager::PaneInfos& LayoutManager::PaneInfos::setDefaultOpened(const bool vOpened) {
    openedDefault = vOpened;
    return *this;
}
LayoutManager::PaneInfos& LayoutManager::PaneInfos::setDefaultFocused(const bool vFocused) {
    focusedDefault = vFocused;
    return *this;
}
void LayoutManager::PaneInfos::m_finalize() {
    if (paneMenuName.empty()) {
        paneMenuName = paneName;
    }
    if (paneDisposal == "CENTRAL") {
        paneDisposalRatio = 0.0f;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///// LayoutManager /////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LayoutManager::LayoutManager() = default;
LayoutManager::~LayoutManager() = default;

bool LayoutManager::AddPane(const PaneInfos& vPaneInfos) {
    static int32_t sMaxPanes = static_cast<int32_t>(sizeof(LayoutPaneFlag) * 8U);
    if (m_FlagCount < sMaxPanes) {
        LayoutPaneFlag flag = (static_cast<LayoutPaneFlag>(1) << (++m_FlagCount));
        if (vPaneInfos.ilayoutPane.expired()) {
            return false;
        }
        if (vPaneInfos.paneName.empty()) {
            return false;
        }
        if (m_PanesByName.find(vPaneInfos.paneName) != m_PanesByName.end()) {
            return false;  // pane name not already exist
        }
        if (m_PanesByFlag.find(flag) != m_PanesByFlag.end()) {
            return false;  // pane flag not already exist
        }
        auto pane_ptr = vPaneInfos.ilayoutPane.lock();
        if (pane_ptr != nullptr) {
            pane_ptr->SetName(vPaneInfos.paneName);
            pane_ptr->SetFlag(flag);  // this can be done only this time.
            auto internal_pane_ptr = std::make_shared<PaneInfos>(vPaneInfos);
            internal_pane_ptr->m_finalize();
            internal_pane_ptr->m_paneFlag = flag;
            if (internal_pane_ptr->openedDefault) {
                m_paneOpenedDefault |= internal_pane_ptr->m_paneFlag;
            }
            if (internal_pane_ptr->focusedDefault) {
                m_paneFocusedDefault |= internal_pane_ptr->m_paneFlag;
            }
            m_PanesDisposalRatios[internal_pane_ptr->paneDisposal] = internal_pane_ptr->paneDisposalRatio;
            m_PanesByDisposal[internal_pane_ptr->paneDisposal].push_back(internal_pane_ptr);
            m_PanesByName[internal_pane_ptr->paneName] = internal_pane_ptr;
            m_PanesByFlag[internal_pane_ptr->m_paneFlag] = internal_pane_ptr;
            m_PanesInDisplayOrder[internal_pane_ptr->paneMenuCategoryName].push_back(internal_pane_ptr);
            m_OrderesPanes.push_back(internal_pane_ptr);
        }
        return true;
    }
    throw LayoutManagerException("Only %u max panes are possible", (uint32_t)sMaxPanes);
    return false;
}

void LayoutManager::RemovePane(const LayoutPaneName& vName) {
    if (m_PanesByName.find(vName) != m_PanesByName.end()) {
        auto pane_ptr = m_PanesByName.at(vName);
        if (pane_ptr != nullptr) {
            // we destroy the weak pointers
            if (m_PanesByDisposal.find(pane_ptr->paneDisposal) != m_PanesByDisposal.end()) {
                auto& arr = m_PanesByDisposal.at(pane_ptr->paneDisposal);
                size_t idx = 0U;
                std::set<size_t> indexs;
                for (const auto& pane : arr) {
                    auto ptr = pane.lock();
                    if (ptr != nullptr && pane_ptr == ptr) {
                        indexs.emplace(idx);
                    }
                    ++idx;
                }
                for (auto id : indexs) {
                    arr.erase(arr.begin() + id);
                }
            }
            if (m_PanesByFlag.find(pane_ptr->m_paneFlag) != m_PanesByFlag.end()) {
                m_PanesByFlag.erase(pane_ptr->m_paneFlag);
            }
            if (m_PanesInDisplayOrder.find(pane_ptr->paneMenuCategoryName) != m_PanesInDisplayOrder.end()) {
                auto& arr = m_PanesInDisplayOrder.at(pane_ptr->paneMenuCategoryName);
                size_t idx = 0U;
                std::set<size_t> indexs;
                for (const auto& pane : arr) {
                    auto ptr = pane.lock();
                    if (ptr != nullptr && pane_ptr == ptr) {
                        indexs.emplace(idx);
                    }
                    ++idx;
                }
                for (auto id : indexs) {
                    arr.erase(arr.begin() + id);
                }
            }
            if (m_paneOpenedDefault & pane_ptr->m_paneFlag) {
                m_paneOpenedDefault &= ~pane_ptr->m_paneFlag;
            }
            if (m_paneFocusedDefault & pane_ptr->m_paneFlag) {
                m_paneFocusedDefault &= ~pane_ptr->m_paneFlag;
            }
            // we do that at end, because this will destroy the shared pointer
            m_PanesByName.erase(pane_ptr->paneName);
            for (auto pane = m_OrderesPanes.begin(); pane != m_OrderesPanes.end(); ++pane) {
                if (pane->expired()) {
                    m_OrderesPanes.erase(pane);
                    break;
                }
            }
        }
    }
}

void LayoutManager::SetPaneDisposalRatio(const PaneDisposal& vPaneDisposal, const float vRatio) {
    if (!vPaneDisposal.empty()) {
        if (vPaneDisposal != "LEFT" && vPaneDisposal != "RIGHT" && vPaneDisposal != "TOP" && vPaneDisposal != "BOTTOM") {
            throw LayoutManagerException("bad split name \"%s\". must be CENTRAL, LEFT, RIGHT, TOP or BOTTOM", vPaneDisposal.c_str());
        }
        m_PanesDisposalRatios[vPaneDisposal] = vRatio;
    }
}

void LayoutManager::Init(const std::string& vMenuLabel, const std::string& vDefaultMenuLabel, const bool vForceDefaultLayout) {
    assert(!vMenuLabel.empty());
    assert(!vDefaultMenuLabel.empty());
    m_MenuLabel = vMenuLabel;
    m_DefaultMenuLabel = vDefaultMenuLabel;
    ImGuiContext& g = *GImGui;
    if (vForceDefaultLayout) {
        m_FirstLayout = true;
    } else {
        ImFileHandle f;
        if ((f = ImFileOpen(g.IO.IniFilename, "r")) == NULL) {
            m_FirstLayout = true;  // need default layout
        }
    }
}

void LayoutManager::Unit() {
    m_PanesByDisposal.clear();
    m_PanesByName.clear();
    m_PanesInDisplayOrder.clear();
    m_PanesByFlag.clear();
    m_PanesDisposalRatios.clear();
    m_OrderesPanes.clear();
}

bool LayoutManager::InitPanes() {
    bool res = true;
    for (const auto& pane : m_PanesByFlag) {
        auto pane_ptr = pane.second.lock();
        if (pane_ptr != nullptr) {
            auto layoutPanePtr = pane_ptr->ilayoutPane.lock();
            if (layoutPanePtr != nullptr) {
                res &= layoutPanePtr->Init();
            }
        }
    }
    return res;
}

void LayoutManager::UnitPanes() {
    for (const auto& pane : m_PanesByFlag) {
        auto pane_ptr = pane.second.lock();
        if (pane_ptr) {
            auto layoutPanePtr = pane_ptr->ilayoutPane.lock();
            if (layoutPanePtr != nullptr) {
                layoutPanePtr->Unit();
            }
        }
    }
}

void LayoutManager::InitAfterFirstDisplay(const ImVec2& vSize) {
    if (m_FirstLayout) {
        ApplyInitialDockingLayout(vSize);
        m_FirstLayout = false;
    }
    if (m_FirstStart) {
        // focus after start of panes
        m_SetFocusedPanes(m_paneFocused);
        m_FirstStart = false;
    }
}

bool LayoutManager::BeginDockSpace(const ImGuiDockNodeFlags& vFlags, const ImVec2& voffset) {
    const auto viewport = ImGui::GetMainViewport();
    m_lastViewportSize = viewport->Size;
    ImGui::SetNextWindowPos(viewport->WorkPos + voffset);
    ImGui::SetNextWindowSize(viewport->WorkSize - voffset);
    ImGui::SetNextWindowViewport(viewport->ID);
    auto host_window_flags = 0;
    host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
    host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    if (vFlags & ImGuiDockNodeFlags_PassthruCentralNode) {
        host_window_flags |= ImGuiWindowFlags_NoBackground;
    }
    char label[100 + 1];
    ImFormatString(label, 100, "DockSpaceViewport_%08X", viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    const auto res = ImGui::Begin(label, nullptr, host_window_flags);
    ImGui::PopStyleVar(3);
    m_DockSpaceID = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(m_DockSpaceID, ImVec2(0.0f, 0.0f), vFlags);
    return res;
}

void LayoutManager::EndDockSpace() {
    ImGui::End();
}

bool LayoutManager::IsDockSpaceHoleHovered() {
    auto central_node_ptr = ImGui::DockBuilderGetCentralNode(m_DockSpaceID);
    if (central_node_ptr != nullptr) {
        return ImGui::IsMouseHoveringRect(central_node_ptr->Pos, central_node_ptr->Pos + central_node_ptr->Size, true);
    }
    return false;
}

void LayoutManager::ApplyInitialDockingLayout(const ImVec2& vSize) {
    ImVec2 _size = vSize;
    if (IS_FLOAT_EQUAL(_size.x, 0.0f) || IS_FLOAT_EQUAL(_size.y, 0.0f)) {
        if (IS_FLOAT_EQUAL(m_lastViewportSize.x, 0.0f) || IS_FLOAT_EQUAL(m_lastViewportSize.y, 0.0f)) {
            return;
        }
        _size = m_lastViewportSize;
    }
    ImGui::DockBuilderRemoveNode(m_DockSpaceID);                             // Clear out existing layout
    ImGui::DockBuilderAddNode(m_DockSpaceID, ImGuiDockNodeFlags_DockSpace);  // Add empty node
    ImGui::DockBuilderSetNodeSize(m_DockSpaceID, _size);
    float leftColumnRatio = 0.3f;
    if (m_PanesDisposalRatios.find("LEFT") != m_PanesDisposalRatios.end()) {
        leftColumnRatio = m_PanesDisposalRatios.at("LEFT");
    }
    float rightColumnRatio = 0.3f;
    if (m_PanesDisposalRatios.find("RIGHT") != m_PanesDisposalRatios.end()) {
        rightColumnRatio = m_PanesDisposalRatios.at("RIGHT");
    }
    float bottomColumnRatio = 0.3f;
    if (m_PanesDisposalRatios.find("BOTTOM") != m_PanesDisposalRatios.end()) {
        bottomColumnRatio = m_PanesDisposalRatios.at("BOTTOM");
    }
    float topColumnRatio = 0.3f;
    if (m_PanesDisposalRatios.find("TOP") != m_PanesDisposalRatios.end()) {
        topColumnRatio = m_PanesDisposalRatios.at("TOP");
    }
    std::unordered_map<PaneDisposal, ImGuiID> guiids;
    // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
    auto dockMainID = m_DockSpaceID;
    // a first split is needed
    guiids["LEFT"] = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, leftColumnRatio, nullptr, &dockMainID);
    guiids["RIGHT"] = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Right, rightColumnRatio, nullptr, &dockMainID);
    guiids["TOP"] = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Up, topColumnRatio, nullptr, &dockMainID);
    guiids["BOTTOM"] = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, bottomColumnRatio, nullptr, &dockMainID);
    for (const auto& pane : m_OrderesPanes) {
        auto pane_ptr = pane.lock();
        if (pane_ptr != nullptr) {
            auto arr = m_ParsePaneDisposal(pane_ptr->paneDisposal);
            if (!arr.empty()) {
                ImGuiDir dir = ImGuiDir_None;
                size_t idx = 0U;
                std::string path = arr[0];
                for (const auto& a : arr) {
                    if (a == "CENTRAL") {
                        ImGui::DockBuilderDockWindow(pane_ptr->paneName.c_str(), dockMainID);
                        break;
                    } else {
                        if (a != "LEFT" && a != "RIGHT" && a != "TOP" && a != "BOTTOM") {
                            std::string msg = "bad split name \"" + a + "\" for pane \"" + pane_ptr->paneName + "\". must be CENTRAL, LEFT, RIGHT, TOP or BOTTOM";
                            throw LayoutManagerException(msg);
                        }
                        if (idx++ > 0) {
                            if (a == "LEFT") {
                                dir = ImGuiDir_Left;  // 0
                            } else if (a == "RIGHT") {
                                dir = ImGuiDir_Right;  // 1
                            } else if (a == "TOP") {
                                dir = ImGuiDir_Up;  // 2
                            } else if (a == "BOTTOM") {
                                dir = ImGuiDir_Down;  // 3
                            }
                            auto guiid_to_split = guiids[path];
                            path += "/" + a;
                            guiids[path] = ImGui::DockBuilderSplitNode(guiid_to_split, dir, pane_ptr->paneDisposalRatio, nullptr, &guiid_to_split);
                        }
                        ImGui::DockBuilderDockWindow(pane_ptr->paneName.c_str(), guiids[path]);
                    }
                }
            }
        }
    }
    ImGui::DockBuilderFinish(m_DockSpaceID);
    m_paneShown = m_paneOpenedDefault;  // will show when pane will be passed
    m_paneFocused = m_paneFocusedDefault;
    m_FirstStart = true;
}

template <typename T>
static bool LayoutManager_MenuItem(const char* label, const char* shortcut, T* vContainer, T vFlag, bool vOnlyOneSameTime = false) {
    bool selected = *vContainer & vFlag;
    const bool res = ImGui::MenuItem(label, shortcut, &selected, true);
    if (res) {
        if (selected) {
            if (vOnlyOneSameTime) {
                *vContainer = vFlag;  // set
            } else {
                *vContainer = (T)(*vContainer | vFlag);  // add
            }
        } else if (!vOnlyOneSameTime) {
            *vContainer = (T)(*vContainer & ~vFlag);  // remove
        }
    }
    return res;
}

void LayoutManager::DisplayMenu(const ImVec2& vSize) {
    if (ImGui::BeginMenu(m_MenuLabel.c_str())) {
        if (ImGui::MenuItem(m_DefaultMenuLabel.c_str())) {
            ApplyInitialDockingLayout(vSize);
        }
        ImGui::Separator();
        bool _menuOpened = false;
        for (const auto& paneCategory : m_PanesInDisplayOrder) {
            _menuOpened = false;
            if (!paneCategory.first.empty()) {
                _menuOpened = ImGui::BeginMenu(paneCategory.first.c_str());
            }
            if (paneCategory.first.empty() || _menuOpened) {
                for (auto pane : paneCategory.second) {
                    auto pane_ptr = pane.lock();
                    if (pane_ptr != nullptr) {
                        auto layoutPanePtr = pane_ptr->ilayoutPane.lock();
                        if (layoutPanePtr != nullptr && layoutPanePtr->CanBeDisplayed()) {
                            LayoutManager_MenuItem<LayoutPaneFlag>(pane_ptr->paneMenuName.c_str(), "", &m_paneShown, pane_ptr->m_paneFlag);
                        }
                    }
                }
            }
            if (_menuOpened) {
                ImGui::EndMenu();
            }
        }
        ImGui::EndMenu();
    }
}

bool LayoutManager::DrawPanes(const uint32_t& vCurrentFrame, ImGuiContext* vContextPtr, void* vUserDatas) {
    bool change = false;
    for (const auto& pane : m_PanesByFlag) {
        auto pane_ptr = pane.second.lock();
        if (pane_ptr != nullptr) {
            auto layoutPanePtr = pane_ptr->ilayoutPane.lock();
            if (layoutPanePtr != nullptr && layoutPanePtr->CanBeDisplayed()) {
                // tofix : pane_ptr->m_showPaneAtFirstCall seem to be never set
                if (pane_ptr->m_showPaneAtFirstCall) {
                    ShowSpecificPane(pane_ptr->m_paneFlag);
                    pane_ptr->m_showPaneAtFirstCall = false;
                }
                // tofix : pane_ptr->m_hidePaneAtFirstCall seem to be never set
                if (pane_ptr->m_hidePaneAtFirstCall) {
                    HideSpecificPane(pane_ptr->m_paneFlag);
                    pane_ptr->m_hidePaneAtFirstCall = false;
                }
                bool is_opened = static_cast<bool>(m_paneShown & pane_ptr->m_paneFlag);
                change |= layoutPanePtr->DrawPanes(vCurrentFrame, &is_opened, vContextPtr, vUserDatas);
                if (!is_opened) {
                    HideSpecificPane(pane_ptr->m_paneFlag);
                }
            }
        }
    }
    return change;
}

bool LayoutManager::DrawDialogsAndPopups(const uint32_t& vCurrentFrame, const ImRect& vRect, ImGuiContext* vContextPtr, void* vUserDatas) {
    bool change = false;
    for (const auto& pane : m_PanesByFlag) {
        auto pane_ptr = pane.second.lock();
        if (pane_ptr != nullptr) {
            auto layoutPanePtr = pane_ptr->ilayoutPane.lock();
            if (layoutPanePtr != nullptr && layoutPanePtr->CanBeDisplayed()) {
                change |= layoutPanePtr->DrawDialogsAndPopups(vCurrentFrame, vRect, vContextPtr, vUserDatas);
            }
        }
    }
    return change;
}

bool LayoutManager::DrawWidgets(const uint32_t& vCurrentFrame, ImGuiContext* vContextPtr, void* vUserDatas) {
    bool change = false;
    for (const auto& pane : m_PanesByFlag) {
        auto pane_ptr = pane.second.lock();
        if (pane_ptr != nullptr) {
            auto layoutPanePtr = pane_ptr->ilayoutPane.lock();
            if (layoutPanePtr != nullptr && layoutPanePtr->CanBeDisplayed()) {
                change |= layoutPanePtr->DrawWidgets(vCurrentFrame, vContextPtr, vUserDatas);
            }
        }
    }
    return change;
}

bool LayoutManager::DrawOverlays(const uint32_t& vCurrentFrame, const ImRect& vRect, ImGuiContext* vContextPtr, void* vUserDatas) {
    bool change = false;
    for (const auto& pane : m_PanesByFlag) {
        auto pane_ptr = pane.second.lock();
        if (pane_ptr != nullptr) {
            auto layoutPanePtr = pane_ptr->ilayoutPane.lock();
            if (layoutPanePtr != nullptr && layoutPanePtr->CanBeDisplayed()) {
                change |= layoutPanePtr->DrawOverlays(vCurrentFrame, vRect, vContextPtr, vUserDatas);
            }
        }
    }
    return change;
}

void LayoutManager::ShowSpecificPane(const LayoutPaneFlag vPane) {
    m_paneShown = (LayoutPaneFlag)((int32_t)m_paneShown | (int32_t)vPane);
}

void LayoutManager::HideSpecificPane(const LayoutPaneFlag vPane) {
    m_paneShown = (LayoutPaneFlag)((int32_t)m_paneShown & ~(int32_t)vPane);
}

void LayoutManager::FocusSpecificPane(const LayoutPaneFlag vPane) {
    ShowSpecificPane(vPane);
    if (m_PanesByFlag.find(vPane) != m_PanesByFlag.end()) {
        auto pane_ptr = m_PanesByFlag.at(vPane).lock();
        if (pane_ptr != nullptr) {
            FocusSpecificPane(pane_ptr->paneName);
        }
    }
}

void LayoutManager::ShowAndFocusSpecificPane(const LayoutPaneFlag vPane) {
    ShowSpecificPane(vPane);
    FocusSpecificPane(vPane);
}

bool LayoutManager::IsSpecificPaneFocused(const LayoutPaneFlag vPane) {
    if (m_PanesByFlag.find(vPane) != m_PanesByFlag.end()) {
        auto pane_ptr = m_PanesByFlag.at(vPane).lock();
        if (pane_ptr != nullptr) {
            return IsSpecificPaneFocused(pane_ptr->paneName);
        }
    }
    return false;
}

void LayoutManager::AddSpecificPaneToExisting(const std::string& vNewPane, const std::string& vExistingPane) {
    ImGuiWindow* window_ptr = ImGui::FindWindowByName(vExistingPane.c_str());
    if (window_ptr != nullptr) {
        const auto& dockid = window_ptr->DockId;
        ImGui::DockBuilderDockWindow(vNewPane.c_str(), dockid);
    }
}

///////////////////////////////////////////////////////
//// PRIVATE //////////////////////////////////////////
///////////////////////////////////////////////////////

bool LayoutManager::IsSpecificPaneFocused(const std::string& vlabel) {
    ImGuiWindow* window_ptr = ImGui::FindWindowByName(vlabel.c_str());
    if (window_ptr != nullptr) {
        return window_ptr->DockTabIsVisible || window_ptr->ViewportOwned;
    }
    return false;
}

void LayoutManager::FocusSpecificPane(const std::string& vlabel) {
    ImGuiWindow* window_ptr = ImGui::FindWindowByName(vlabel.c_str());
    if (window_ptr != nullptr) {
        if (!window_ptr->DockTabIsVisible) {
            ImGui::FocusWindow(window_ptr);
        }
    }
}

///////////////////////////////////////////////////////
//// CONFIGURATION PRIVATE ////////////////////////////
///////////////////////////////////////////////////////

LayoutPaneFlag LayoutManager::m_GetFocusedPanes() {
    LayoutPaneFlag flag = 0;
    for (const auto& pane : m_PanesByFlag) {
        auto pane_ptr = pane.second.lock();
        if (pane_ptr != nullptr && IsSpecificPaneFocused(pane_ptr->paneName)) {
            flag = (LayoutPaneFlag)((int32_t)flag | (int32_t)pane.first);
        }
    }
    return flag;
}

void LayoutManager::m_SetFocusedPanes(const LayoutPaneFlag vActivePanes) {
    for (const auto& pane : m_PanesByFlag) {
        auto pane_ptr = pane.second.lock();
        if (pane_ptr != nullptr && ((vActivePanes & pane.first) != 0)) {
            FocusSpecificPane(pane_ptr->paneName);
        }
    }
}

static inline std::vector<std::string> s_splitStringToVector(const std::string& text, const std::string& delimiters, bool pushEmpty) {
    std::vector<std::string> arr;
    if (!text.empty()) {
        std::string::size_type start = 0;
        std::string::size_type end = text.find_first_of(delimiters, start);
        while (end != std::string::npos) {
            const std::string& token = text.substr(start, end - start);
            if (!token.empty() || (token.empty() && pushEmpty)) {
                arr.emplace_back(token);
            }
            start = end + 1;
            end = text.find_first_of(delimiters, start);
        }
        const std::string& token = text.substr(start);
        if (!token.empty() || (token.empty() && pushEmpty)) {
            arr.emplace_back(token);
        }
    }
    return arr;
}

std::vector<std::string> LayoutManager::m_ParsePaneDisposal(const PaneDisposal& vPaneDisposal) {
    return s_splitStringToVector(vPaneDisposal, "/", false);
}

///////////////////////////////////////////////////////
//// CONFIGURATION PUBLIC /////////////////////////////
///////////////////////////////////////////////////////

#ifdef EZ_TOOLS_XML_CONFIG

ez::xml::Nodes LayoutManager::getXmlNodes(const std::string& vUserDatas) {
    ez::xml::Node node("layout");
    if (vUserDatas == "app") {
        m_paneFocused = m_GetFocusedPanes();
#ifdef EZ_TOOLS_VARIANT
        node.addChild("panes")
            .addAttribute("opened", ez::ivariant((int32_t)m_paneShown).GetS())  //
            .addAttribute("active", ez::ivariant((int32_t)m_paneFocused).GetS());
#endif  // EZ_TOOLS_VARIANT
    } else if (vUserDatas == "project") {
        /*str += vOffset + "<layout>\n";
        for (auto pane : m_PanesByName) {
            auto ptr = std::dynamic_pointer_cast<ez::xml::Config>(pane.second.lock());
            if (ptr) {
                str += ptr->getXml(vOffset + "\t", "project");
            }
        }
        str += vOffset + "</layout>\n";*/
    }
    return {node};
}

bool LayoutManager::setFromXmlNodes(const ez::xml::Node& vNode, const ez::xml::Node& vParent, const std::string& vUserDatas) {
    // The value of this child identifies the name of this element
    const auto& strName = vNode.getName();
    const auto& strValue = vNode.getContent();
    const auto& strParentName = vParent.getName();

    if (vUserDatas == "app") {
        if (strParentName == "layout") {
#ifdef EZ_TOOLS_VARIANT
            if (vNode.isAttributeExist("opened")) {
                m_paneShown = (LayoutPaneFlag)ez::ivariant(vNode.getAttribute("opened")).GetI();
            }
            if (vNode.isAttributeExist("active")) {
                m_paneFocused = (LayoutPaneFlag)ez::ivariant(vNode.getAttribute("active")).GetI();
            }
#endif  // EZ_TOOLS_VARIANT
        }
        return true;
    } else if (vUserDatas == "project") {
        if (strParentName == "layout") {
            /*for (auto pane : m_PanesByName) {
                auto ptr = std::dynamic_pointer_cast<ez::xml::Config>(pane.second.lock());
                if (ptr) {
                    ptr->RecursParsingConfig(vNode, vParent , "project");
                }
            }*/
        }
    }
    return false;
}

#endif  // EZ_TOOLS_XML_CONFIG
