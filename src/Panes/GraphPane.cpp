// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "GraphPane.h"
#include <imgui_internal.h>
#include <Graph/Manager/NodeManager.h>
#include <cinttypes>  // printf zu

GraphPane::GraphPane() = default;
GraphPane::~GraphPane() {
    Unit();
}

bool GraphPane::Init() {
    return true;
}

void GraphPane::Unit() {
}

///////////////////////////////////////////////////////////////////////////////////
//// IMGUI PANE ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool GraphPane::DrawPanes(const uint32_t& /*vCurrentFrame*/, PaneFlags& vInOutPaneShown, ImGuiContext* vContextPtr, void* /*vUserDatas*/) {
    ImGui::SetCurrentContext(vContextPtr);
    bool change = false;
    if (vInOutPaneShown & paneFlag) {
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus /* | ImGuiWindowFlags_MenuBar*/;
        if (ImGui::Begin<PaneFlags>(paneName.c_str(), &vInOutPaneShown, paneFlag, flags)) {
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
            auto win = ImGui::GetCurrentWindowRead();
            if (win->Viewport->Idx != 0)
                flags |= ImGuiWindowFlags_NoResize;  // | ImGuiWindowFlags_NoTitleBar;
            else
                flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus /* | ImGuiWindowFlags_MenuBar*/;
#endif
            if (ImGui::BeginMenuBar()) {
                ImGui::EndMenuBar();
            }
            NodeManager::Instance()->drawGraph();
        }

        ImGui::End();
    }
    return change;
}

bool GraphPane::DrawOverlays(const uint32_t& /*vCurrentFrame*/, const ImRect& /*vRect*/, ImGuiContext* vContextPtr, void* /*vUserDatas*/) {
    ImGui::SetCurrentContext(vContextPtr);
    return false;
}

bool GraphPane::DrawDialogsAndPopups(const uint32_t& /*vCurrentFrame*/, const ImVec2& /*vMaxSize*/, ImGuiContext* vContextPtr, void* /*vUserDatas*/) {
    ImGui::SetCurrentContext(vContextPtr);
    return false;
}

bool GraphPane::DrawWidgets(const uint32_t& /*vCurrentFrame*/, ImGuiContext* vContextPtr, void* /*vUserDatas*/) {
    ImGui::SetCurrentContext(vContextPtr);
    return false;
}
