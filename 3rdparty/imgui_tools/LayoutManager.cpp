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
#include <ImGuiPack.h>
#include <cstdarg>  // For va_start, etc.

#include <imgui_internal.h>

class LayoutManagerException : public std::exception {
private:
    std::string m_msg;

public:
    LayoutManagerException() : std::exception() {
    }
    // std::exception(msg) is not availaiable on linux it seems... but on windows yes
    explicit LayoutManagerException(const std::string& vMsg) : std::exception(), m_msg(vMsg) {
    }
    explicit LayoutManagerException(char const* const fmt, ...) : std::exception() {
        va_list args;
        va_start(args, fmt);
        char TempBuffer[1024 + 1];
        const int w = vsnprintf(TempBuffer, 3072, fmt, args);
        va_end(args);
        if (w) {
            m_msg = std::string(TempBuffer, (size_t)w);
        }
    }
    ~LayoutManagerException() override = default;
    char const* what() const noexcept override {
        return m_msg.c_str();
    }
};

LayoutManager::LayoutManager() = default;
LayoutManager::~LayoutManager() = default;

bool LayoutManager::AddPane(ILayoutPaneWeak vPane,
                            const std::string& vName,
                            const PaneCategoryName& vCategory,
                            const PaneDisposal& vPaneDisposal,
                            const float& vPaneDisposalRatio,
                            const bool& vIsOpenedDefault,
                            const bool& vIsFocusedDefault) {
    static auto sMaxPanes = sizeof(LayoutPaneFlag) * 8U;
    if (m_FlagCount < sMaxPanes) {
        LayoutPaneFlag flag = (static_cast<LayoutPaneFlag>(1) << (++m_FlagCount));
        if (vPane.expired()) {
            return false;
        }
        if (vName.empty()) {
            return false;
        }
        if (m_PanesByName.find(vName) != m_PanesByName.end()) {
            return false;  // pane name not already exist
        }
        if (m_PanesByFlag.find(flag) != m_PanesByFlag.end()) {
            return false;  // pane flag not already exist
        }
        auto pane_ptr = vPane.lock();
        if (pane_ptr != nullptr) {
            pane_ptr->SetName(vName);
            pane_ptr->SetFlag(flag);  // this can be done only this time.
            auto internal_pane_ptr = std::make_shared<InternalPane>();
            internal_pane_ptr->ilayoutPane = vPane;
            internal_pane_ptr->paneName = vName;
            internal_pane_ptr->paneFlag = flag;
            internal_pane_ptr->paneCategory = vCategory;
            internal_pane_ptr->paneDisposal = vPaneDisposal;
            internal_pane_ptr->paneDisposalRatio = vPaneDisposalRatio;
            if (vIsOpenedDefault) {
                m_Pane_Opened_Default |= internal_pane_ptr->paneFlag;
            }
            if (vIsFocusedDefault) {
                m_Pane_Focused_Default |= internal_pane_ptr->paneFlag;
            }
            m_PanesDisposalRatios[vPaneDisposal] = internal_pane_ptr->paneDisposalRatio;
            m_PanesByDisposal[vPaneDisposal].push_back(internal_pane_ptr);
            m_PanesByName[vName] = internal_pane_ptr;
            m_PanesByFlag[internal_pane_ptr->paneFlag] = internal_pane_ptr;
            m_PanesInDisplayOrder[vCategory].push_back(internal_pane_ptr);
        }
        return true;
    }
    throw LayoutManagerException("Only %u max panes are possible", (uint32_t)sMaxPanes);
    return false;
}

void LayoutManager::RemovePane(const std::string& vName) {
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
            if (m_PanesByFlag.find(pane_ptr->paneFlag) != m_PanesByFlag.end()) {
                m_PanesByFlag.erase(pane_ptr->paneFlag);
            }
            if (m_PanesInDisplayOrder.find(pane_ptr->paneCategory) != m_PanesInDisplayOrder.end()) {
                auto& arr = m_PanesInDisplayOrder.at(pane_ptr->paneCategory);
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
            if (m_Pane_Opened_Default & pane_ptr->paneFlag) {
                m_Pane_Opened_Default &= ~pane_ptr->paneFlag;
            }
            if (m_Pane_Focused_Default & pane_ptr->paneFlag) {
                m_Pane_Focused_Default &= ~pane_ptr->paneFlag;
            }
            // we do that at end, because this will destroy the shared pointer
            m_PanesByName.erase(pane_ptr->paneName);
        }
    }
}

void LayoutManager::SetPaneDisposalRatio(const PaneDisposal& vPaneDisposal, const float& vRatio) {
    if (!vPaneDisposal.empty()) {
        if (vPaneDisposal != "LEFT" && vPaneDisposal != "RIGHT" && vPaneDisposal != "TOP" && vPaneDisposal != "BOTTOM") {
            throw LayoutManagerException("bad split name \"%s\". must be CENTRAL, LEFT, RIGHT, TOP or BOTTOM", vPaneDisposal.c_str());
        }
        m_PanesDisposalRatios[vPaneDisposal] = vRatio;
    }
}

void LayoutManager::Init(const std::string& vMenuLabel, const std::string& vDefaultMenuLabel, const bool& vForceDefaultLayout) {
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
        m_SetFocusedPanes(m_Pane_Focused);
        m_FirstStart = false;
    }
}

bool LayoutManager::BeginDockSpace(const ImGuiDockNodeFlags& vFlags, const ImVec2& voffset) {
    const auto viewport = ImGui::GetMainViewport();
    m_LastSize = viewport->Size;
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
        if (IS_FLOAT_EQUAL(m_LastSize.x, 0.0f) || IS_FLOAT_EQUAL(m_LastSize.y, 0.0f)) {
            return;
        }
        _size = m_LastSize;
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
    for (const auto& pane : m_PanesByName) {
        auto pane_ptr = pane.second;
        if (pane_ptr != nullptr) {
            auto arr = m_ParsePaneDisposal(pane_ptr->paneDisposal);
            if (!arr.empty()) {
                ImGuiDir dir = ImGuiDir_None;
                size_t idx = 0U;
                std::string path = arr[0];
                for (const auto& a : arr) {
                    if (a == "CENTRAL") {
                        ImGui::DockBuilderDockWindow(pane.first.c_str(), dockMainID);
                        break;
                    } else {
                        if (a != "LEFT" && a != "RIGHT" && a != "TOP" && a != "BOTTOM") {
                            std::string msg = "bad split name \"" + a + "\" for pane \"" + pane.first + "\". must be CENTRAL, LEFT, RIGHT, TOP or BOTTOM";
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
                        ImGui::DockBuilderDockWindow(pane.first.c_str(), guiids[path]);
                    }
                }
            }
        }
    }
    ImGui::DockBuilderFinish(m_DockSpaceID);
    pane_Shown = m_Pane_Opened_Default;  // will show when pane will be passed
    m_Pane_Focused = m_Pane_Focused_Default;
    m_FirstStart = true;
}

template <typename T>
static bool LayoutManager_MenuItem(const char* label, const char* shortcut, T* vContainer, T vFlag, bool vOnlyOneSameTime = false) {
    bool selected = *vContainer & vFlag;
    const bool& res = ImGui::MenuItem(label, shortcut, &selected, true);
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
                            LayoutManager_MenuItem<LayoutPaneFlag>(pane_ptr->paneName.c_str(), "", &pane_Shown, pane_ptr->paneFlag);
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
                if (pane_ptr->showPaneAtFirstCall) {
                    ShowSpecificPane(pane_ptr->paneFlag);
                    pane_ptr->showPaneAtFirstCall = false;
                }
                if (pane_ptr->hidePaneAtFirstCall) {
                    HideSpecificPane(pane_ptr->paneFlag);
                    pane_ptr->hidePaneAtFirstCall = false;
                }
                bool is_opened = static_cast<bool>(pane_Shown & pane_ptr->paneFlag);
                change |= layoutPanePtr->DrawPanes(vCurrentFrame, &is_opened, vContextPtr, vUserDatas);
                if (!is_opened) {
                    HideSpecificPane(pane_ptr->paneFlag);
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

void LayoutManager::ShowSpecificPane(const LayoutPaneFlag& vPane) {
    pane_Shown = (LayoutPaneFlag)((int32_t)pane_Shown | (int32_t)vPane);
}

void LayoutManager::HideSpecificPane(const LayoutPaneFlag& vPane) {
    pane_Shown = (LayoutPaneFlag)((int32_t)pane_Shown & ~(int32_t)vPane);
}

void LayoutManager::FocusSpecificPane(const LayoutPaneFlag& vPane) {
    ShowSpecificPane(vPane);
    if (m_PanesByFlag.find(vPane) != m_PanesByFlag.end()) {
        auto pane_ptr = m_PanesByFlag.at(vPane).lock();
        if (pane_ptr != nullptr) {
            FocusSpecificPane(pane_ptr->paneName);
        }
    }
}

void LayoutManager::ShowAndFocusSpecificPane(const LayoutPaneFlag& vPane) {
    ShowSpecificPane(vPane);
    FocusSpecificPane(vPane);
}

bool LayoutManager::IsSpecificPaneFocused(const LayoutPaneFlag& vPane) {
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

void LayoutManager::m_SetFocusedPanes(const LayoutPaneFlag& vActivePanes) {
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

#ifdef USE_XML_CONFIG

std::string LayoutManager::getXml(const std::string& vOffset, const std::string& vUserDatas) {
    std::string str;
    if (vUserDatas == "app") {
        str += vOffset + "<layout>\n";
        m_Pane_Focused = m_GetFocusedPanes();
        str += vOffset + "\t<panes opened=\"" + ct::ivariant((int32_t)pane_Shown).GetS() + "\" active=\"" + ct::ivariant((int32_t)m_Pane_Focused).GetS() + "\"/>\n";
        str += vOffset + "</layout>\n";
    } else if (vUserDatas == "project") {
        /*str += vOffset + "<layout>\n";
        for (auto pane : m_PanesByName) {
            auto ptr = std::dynamic_pointer_cast<conf::ConfigAbstract>(pane.second.lock());
            if (ptr) {
                str += ptr->getXml(vOffset + "\t", "project");
            }
        }
        str += vOffset + "</layout>\n";*/
    }
    return str;
}

bool LayoutManager::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) {
    // The value of this child identifies the name of this element
    std::string strValue = "";
    std::string strParentName = "";
    std::string strName = vElem->Value();
    if (vElem->GetText() != nullptr) {
        strValue = vElem->GetText();
    }
    if (vParent != 0) {
        strParentName = vParent->Value();
    }
    if (vUserDatas == "app") {
        if (strParentName == "layout") {
            for (const tinyxml2::XMLAttribute* attr = vElem->FirstAttribute(); attr != nullptr; attr = attr->Next()) {
                std::string attName = attr->Name();
                std::string attValue = attr->Value();

                if (attName == "opened") {
                    pane_Shown = (LayoutPaneFlag)ct::ivariant(attValue).GetI();
                } else if (attName == "active") {
                    m_Pane_Focused = (LayoutPaneFlag)ct::ivariant(attValue).GetI();
                }
            }
        }
        return true;
    } else if (vUserDatas == "project") {
        if (strParentName == "layout") {
            /*for (auto pane : m_PanesByName) {
                auto ptr = std::dynamic_pointer_cast<conf::ConfigAbstract>(pane.second.lock());
                if (ptr) {
                    ptr->RecursParsingConfig(vElem, vParent , "project");
                }
            }*/
        }
    }
    return false;
}

#endif  // USE_XML_CONFIG
