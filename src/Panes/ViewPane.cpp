// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ViewPane.h"
#include <imgui_internal.h>
#include <cinttypes>  // printf zu

ViewPane::ViewPane() = default;
ViewPane::~ViewPane() {
    Unit();
}

bool ViewPane::Init() {
    return true;
}

void ViewPane::Unit() {
}

///////////////////////////////////////////////////////////////////////////////////
//// IMGUI PANE ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool ViewPane::DrawPanes(const uint32_t& /*vCurrentFrame*/, PaneFlags& vInOutPaneShown, ImGuiContext* vContextPtr, void* /*vUserDatas*/) {
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
        }

        ImGui::End();
    }
    return change;
}

bool ViewPane::DrawOverlays(const uint32_t& /*vCurrentFrame*/, const ImRect& /*vRect*/, ImGuiContext* vContextPtr, void* /*vUserDatas*/) {
    ImGui::SetCurrentContext(vContextPtr);
    return false;
}

bool ViewPane::DrawDialogsAndPopups(const uint32_t& /*vCurrentFrame*/, const ImVec2& /*vMaxSize*/, ImGuiContext* vContextPtr, void* /*vUserDatas*/) {
    ImGui::SetCurrentContext(vContextPtr);
    return false;
}

bool ViewPane::DrawWidgets(const uint32_t& /*vCurrentFrame*/, ImGuiContext* vContextPtr, void* /*vUserDatas*/) {
    ImGui::SetCurrentContext(vContextPtr);
    return false;
}
