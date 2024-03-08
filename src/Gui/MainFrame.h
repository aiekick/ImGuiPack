#pragma once

#include <ImGuiPack.h>

class MainFrame {
private:
    bool m_showImGui = false;
    bool m_showMetrics = false;

public:
    bool init();
    void unit();
    void display(const ImVec2& vPos, const ImVec2& vSize);

private:
    void m_drawMainMenuBar(const ImVec2& vPos, const ImVec2& vSize);
    void m_drawMainStatusBar(const ImVec2& vPos, const ImVec2& vSize);
    void m_buildThemes();

public:
    static MainFrame* Instance() {
        static MainFrame _instance;
        return &_instance;
    }
};
