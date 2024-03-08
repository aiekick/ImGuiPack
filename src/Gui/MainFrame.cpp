#include "MainFrame.h"
#include <ImGuiPack.h>

#include <Panes/CodePane.h>
#include <Panes/ConsolePane.h>
#include <Panes/GraphPane.h>
#include <Panes/ViewPane.h>

#include <sstream> // std::stringstream
#include <iomanip> // std::setprecision

bool MainFrame::init() {
    m_buildThemes();
    ImGui::CustomStyle::Init();

    LayoutManager::Instance()->Init("Layouts", "Default Layout", true);

    LayoutManager::Instance()->AddPane(CodePane::Instance(), "Code Pane", "", "RIGHT", 0.3f, true, false);
    LayoutManager::Instance()->AddPane(GraphPane::Instance(), "Graph Pane", "", "CENTRAL", 0.0f, true, true);
    LayoutManager::Instance()->AddPane(ConsolePane::Instance(), "Console Pane", "", "BOTTOM", 0.3f, false, false);
    LayoutManager::Instance()->AddPane(ViewPane::Instance(), "View Pane", "", "RIGHT/BOTTOM", 0.3f, true, false);

	return true;
}

void MainFrame::unit() {

}

void MainFrame::display(const ImVec2& vPos, const ImVec2& vSize) {
        m_drawMainMenuBar(vPos, vSize);
        m_drawMainStatusBar(vPos, vSize);

        if (LayoutManager::Instance()->BeginDockSpace(ImGuiDockNodeFlags_PassthruCentralNode)) {
            LayoutManager::Instance()->EndDockSpace();
        }

        if (LayoutManager::Instance()->DrawPanes(0, ImGui::GetCurrentContext(), {})) {
        }

        ImGuiThemeHelper::Instance()->Draw();
        LayoutManager::Instance()->InitAfterFirstDisplay(vSize);
}

void MainFrame::m_drawMainMenuBar(const ImVec2& vPos, const ImVec2& vSize) {
    if (ImGui::BeginMainMenuBar()) {
        LayoutManager::Instance()->DisplayMenu(ImGui::GetIO().DisplaySize);

        ImGui::Spacing();

        if (ImGui::BeginMenu("Settings")) {
            if (ImGui::BeginMenu("Styles")) {
                ImGuiThemeHelper::Instance()->DrawMenu();

                ImGui::Separator();

                ImGui::MenuItem("Show ImGui", "", &m_showImGui);
                ImGui::MenuItem("Show ImGui Metric/Debug", "", &m_showMetrics);

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        // ImGui Infos
        std::stringstream sstr;
        sstr << "Dear ImGui " << ImGui::GetVersion() << " (Docking)";
        std::string label = sstr.str();
        const auto size = ImGui::CalcTextSize(label.c_str());
        ImGui::Spacing(ImGui::GetContentRegionAvail().x - size.x - ImGui::GetStyle().FramePadding.x * 2.0f);
        ImGui::Text("%s", label.c_str());

        ImGui::EndMainMenuBar();
    }
}

void MainFrame::m_drawMainStatusBar(const ImVec2& vPos, const ImVec2& vSize) {
    if (ImGui::BeginMainStatusBar()) {
        Messaging::Instance()->DrawStatusBar();

        //  ImGui Infos
        const auto& io = ImGui::GetIO();
        std::stringstream sstr;
        sstr << std::setprecision(1) << std::fixed << 1000.0f / io.Framerate << " ms/frame (" << io.Framerate << " fps)";
        std::string label = sstr.str();
        const auto size = ImGui::CalcTextSize(label.c_str());
        ImGui::Spacing(ImGui::GetContentRegionAvail().x - size.x - ImGui::GetStyle().FramePadding.x * 2.0f);
        ImGui::Text("%s", label.c_str());

        ImGui::EndMainStatusBar();
    }
}
