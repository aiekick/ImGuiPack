#pragma once
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

#include <ImGuiPack.h>

#include <map>
#include <set>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

class IMGUI_API ImCanvas {
public:
    struct Config {
        float zoomMax = 5.0f;
        float zoomMin = 0.25f;
        bool zoomEnabled = true;
        float defaultZoom = 1.0f;
        float zoomDivisions = 5.0f;
        float zoomSmoothness = 2.5f;
        ImVec2 size = {0.0f, 0.0f};
        ImU32 color = IM_COL32_WHITE;
        ImGuiKey resetZoomKey = ImGuiKey_R;
        ImGuiMouseButton scrollButton = ImGuiMouseButton_Middle;
    };

    struct GridConfig {
        ImVec2 gridSize{50.0f, 50.0f};
        ImVec2 gridSubdivs{5.0f, 5.0f};
        ImU32 gridColor{IM_COL32(200, 200, 200, 40)};
        ImU32 subGridColor{IM_COL32(200, 200, 200, 10)};
    };

public:
    ImCanvas();
    ~ImCanvas();
    void begin();
    void end();
    void drawGrid(const GridConfig& vGridConfig);
    float getScale() const;
    const ImVec2& getOrigin() const;
    bool isHovered() const;
    const ImVec2& getScroll() const;
    ImGuiContext* getRawContext();
    Config& getConfigRef();
    ImVec2 screenToCanvas(const ImVec2& vPos, ImGuiContext* vCtx = nullptr);
    ImVec2 canvasToScreen(const ImVec2& vPos, ImGuiContext* vCtx = nullptr);

private:
    void m_CopyIOEvents(ImGuiContext* src, ImGuiContext* dst, ImVec2 origin, float scale);
    void m_AppendDrawData(ImDrawList* src, ImVec2 origin, float scale);

private:
    Config m_Config;
    ImVec2 m_Origin;
    ImVec2 m_Pos;
    ImGuiContext* m_Ctx = nullptr;
    ImGuiContext* m_OriginalCtx = nullptr;
    bool m_AnyWindowHovered = false;
    bool m_AnyItemActive = false;
    bool m_Hovered = false;
    float m_Scale = m_Config.defaultZoom, m_ScaleTarget = m_Config.defaultZoom;
    ImVec2 m_Scroll = {0.f, 0.f}, m_ScrollTarget = {0.f, 0.f};
};
