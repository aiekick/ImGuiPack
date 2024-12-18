#pragma once
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

#include <ImGuiPack.h>

// based on imgui_canvas of TheDmd at https://github.com/thedmd/imgui-node-editor.git

class IMGUI_API ImCanvas2 {
public:
    struct CanvasView {
        ImVec2 Origin;
        float Scale = 1.0f;
        float InvScale = 1.0f;
        CanvasView() = default;
        CanvasView(const ImVec2& origin, float scale) : Origin(origin), Scale(scale), InvScale(scale ? 1.0f / scale : 0.0f) {}
        void set(const ImVec2& origin, float scale) { *this = CanvasView(origin, scale); }
    };

    struct Config {
        float zoomFactor = 1.0f;
        float zoomStep = 0.1f;
        ImGuiKey resetZoomKey = ImGuiKey_R;
        ImGuiMouseButton draggingButton = ImGuiMouseButton_Middle;
        ImGuiMouseButton contextMenuButton = ImGuiMouseButton_Right;
        ImVec2 gridSize{50.0f, 50.0f};
        ImVec2 gridSubdivs{5.0f, 5.0f};
        ImU32 gridColor{IM_COL32(200, 200, 200, 40)};
        ImU32 subGridColor{IM_COL32(200, 200, 200, 10)};
    };

private:
    Config m_config;
    bool m_isDragging{false};
    ImVec2 m_drawStartPoint;
    int m_externalChannel{0};
    bool m_backgroundClicked{false};
    bool m_backgroundDoubleClicked{false};
    bool m_requestContextMenuBackground{false};

public:
    ImCanvas2();
    ~ImCanvas2();

    bool begin(const char* id, const ImVec2& size);
    bool begin(ImGuiID id, const ImVec2& size);
    void end();

    void setView(const ImVec2& origin, float scale);
    void setView(const CanvasView& view);
    void centerView(const ImVec2& canvasPoint);
    CanvasView calcCenterView(const ImVec2& canvasPoint) const;
    void centerView(const ImRect& canvasRect);
    CanvasView calcCenterView(const ImRect& canvasRect) const;

    void suspend();
    bool isSuspended() const { return m_suspendCounter > 0; }
    void resume();

    void setExternalChannel(int32_t vChannel) { m_externalChannel = vChannel; }
    int32_t getExternalChannel() { return m_externalChannel; }

    ImVec2 canvasToScreen(const ImVec2& point) const;
    ImVec2 canvasToScreen(const ImVec2& point, const CanvasView& view) const;

    ImVec2 canvasToScreenV(const ImVec2& vector) const;
    ImVec2 canvasToScreenV(const ImVec2& vector, const CanvasView& view) const;

    ImVec2 screenToCanvas(const ImVec2& point) const;
    ImVec2 screenToCanvas(const ImVec2& point, const CanvasView& view) const;

    ImVec2 screenToCanvasV(const ImVec2& vector) const;
    ImVec2 screenToCanvasV(const ImVec2& vector, const CanvasView& view) const;

    const ImRect& getRect() const { return m_widgetRect; }
    const ImRect& getViewRect() const { return m_viewRect; }
    ImRect calcViewRect(const CanvasView& view) const;
    const CanvasView& getView() const { return m_view; }
    CanvasView& getViewRef() { return m_view; }
    const ImVec2& getViewOrigin() const { return m_view.Origin; }
    float getViewScale() const { return m_view.Scale; }

private:
    void m_resetActions();
    void m_manageActions();
    void m_manageContextMenus();
    void m_manageZoom();
    void m_manageDragging();
    void m_drawGrid();

    void m_updateViewTransformPosition();

    void m_saveInputState();
    void m_restoreInputState();

    void m_saveViewportState();
    void m_restoreViewportState();

    void m_enterLocalSpace();
    void m_leaveLocalSpace();

    bool m_inBeginEnd = false;

    ImVec2 m_widgetPosition;
    ImVec2 m_widgetSize;
    ImRect m_widgetRect;

    ImDrawList* m_drawList = nullptr;
    int m_expectedChannel = 0;

    int m_drawListCommadBufferSize = 0;
    int m_drawListStartVertexIndex = 0;

    CanvasView m_view;
    ImRect m_viewRect;

    ImVec2 m_viewTransformPosition;

    int m_suspendCounter = 0;

    float m_lastFringeScale = 1.0f;

    ImVec2 m_MousePosBackup;
    ImVec2 m_MousePosPrevBackup;
    ImVec2 m_MouseClickedPosBackup[IM_ARRAYSIZE(ImGuiIO::MouseClickedPos)];
    ImVec2 m_windowCursorMaxBackup;

    ImVec2 m_windowPosBackup;
    ImVec2 m_viewportPosBackup;
    ImVec2 m_viewportSizeBackup;
    ImVec2 m_viewportWorkPosBackup;
    ImVec2 m_viewportWorkSizeBackup;
};
