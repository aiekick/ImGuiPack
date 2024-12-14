#include "ImCanvas.h"

ImCanvas::ImCanvas() = default;

ImCanvas::~ImCanvas() {
    if (m_Ctx) {
        ImGui::DestroyContext(m_Ctx);
    }
}

ImCanvas::Config& ImCanvas::getConfigRef() {
    return m_Config;
}

float ImCanvas::getScale() const {
    return m_Scale;
}

const ImVec2& ImCanvas::getOrigin() const {
    return m_Origin;
}

bool ImCanvas::isHovered() const {
    return m_Hovered;
}

const ImVec2& ImCanvas::getScroll() const {
    return m_Scroll;
}

ImGuiContext* ImCanvas::getRawContext() {
    return m_Ctx;
}

ImVec2 ImCanvas::screenToCanvas(const ImVec2& vPos, ImGuiContext* vCtx) {
    if (vCtx == getRawContext()) {
        return vPos - getScroll();
    }
    return vPos - getOrigin() - getScroll() * getScale();
}

ImVec2 ImCanvas::canvasToScreen(const ImVec2& vPos, ImGuiContext* vCtx) {
    if (vCtx == getRawContext()) {
        return vPos + getScroll();
    }
    return vPos + getOrigin() + getScroll() * getScale();
}

void ImCanvas::begin() {
    ImGui::PushID(this);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, m_Config.color);
    ImGui::BeginChild("view_port", m_Config.size, 0, ImGuiWindowFlags_NoMove);
    ImGui::PopStyleColor();
    // m_size = ImGui::GetWindowSize();
    m_Pos = ImGui::GetWindowPos();

    ImVec2 size = ImGui::GetContentRegionAvail();
    m_Origin = ImGui::GetCursorScreenPos();
    m_OriginalCtx = ImGui::GetCurrentContext();
    const ImGuiStyle& orig_style = ImGui::GetStyle();
    if (!m_Ctx)
        m_Ctx = ImGui::CreateContext(ImGui::GetIO().Fonts);
    ImGui::SetCurrentContext(m_Ctx);
    ImGuiStyle& new_style = ImGui::GetStyle();
    new_style = orig_style;

    m_CopyIOEvents(m_OriginalCtx, m_Ctx, m_Origin, m_Scale);

    ImGui::GetIO().DisplaySize = size / m_Scale;
    ImGui::GetIO().ConfigInputTrickleEventQueue = false;
    ImGui::NewFrame();

    if (!m_Config.extraWindowWrapper) {
        return;
    }
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->WorkSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("viewport_container",
                 nullptr,
                 ImGuiWindowFlags_NoDecoration |  //
                     ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::PopStyleVar();
}

void ImCanvas::end() {
    m_AnyWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
    if (m_Config.extraWindowWrapper && ImGui::IsWindowHovered())
        m_AnyWindowHovered = false;

    m_AnyItemActive = ImGui::IsAnyItemActive();

    if (m_Config.extraWindowWrapper)
        ImGui::End();

    ImGui::Render();

    ImDrawData* draw_data = ImGui::GetDrawData();

    ImGui::SetCurrentContext(m_OriginalCtx);
    m_OriginalCtx = nullptr;

    for (int i = 0; i < draw_data->CmdListsCount; ++i) {
        m_AppendDrawData(draw_data->CmdLists[i], m_Origin, m_Scale);
    }

    m_Hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && !m_AnyWindowHovered;

    // Zooming
    if (m_Config.zoomEnabled && m_Hovered && ImGui::GetIO().MouseWheel != 0.f) {
        m_ScaleTarget += ImGui::GetIO().MouseWheel / m_Config.zoomDivisions;
        m_ScaleTarget = m_ScaleTarget < m_Config.zoomMin ? m_Config.zoomMin : m_ScaleTarget;
        m_ScaleTarget = m_ScaleTarget > m_Config.zoomMax ? m_Config.zoomMax : m_ScaleTarget;

        if (m_Config.zoomSmoothness == 0.f) {
            m_Scroll += (ImGui::GetMousePos() - m_Pos) / m_ScaleTarget - (ImGui::GetMousePos() - m_Pos) / m_Scale;
            m_Scale = m_ScaleTarget;
        }
    }
    if (abs(m_ScaleTarget - m_Scale) >= 0.015f / m_Config.zoomSmoothness) {
        float cs = (m_ScaleTarget - m_Scale) / m_Config.zoomSmoothness;
        m_Scroll += (ImGui::GetMousePos() - m_Pos) / (m_Scale + cs) - (ImGui::GetMousePos() - m_Pos) / m_Scale;
        m_Scale += (m_ScaleTarget - m_Scale) / m_Config.zoomSmoothness;

        if (abs(m_ScaleTarget - m_Scale) < 0.015f / m_Config.zoomSmoothness) {
            m_Scroll += (ImGui::GetMousePos() - m_Pos) / m_ScaleTarget - (ImGui::GetMousePos() - m_Pos) / m_Scale;
            m_Scale = m_ScaleTarget;
        }
    }

    // Zoom reset
    if (ImGui::IsKeyPressed(m_Config.resetZoomKey, false))
        m_ScaleTarget = m_Config.defaultZoom;

    // Scrolling
    if (m_Hovered && !m_AnyItemActive && ImGui::IsMouseDragging(m_Config.scrollButton, 0.f)) {
        m_Scroll += ImGui::GetIO().MouseDelta / m_Scale;
        m_ScrollTarget = m_Scroll;
    }

    ImGui::EndChild();
    ImGui::PopID();
}

void ImCanvas::drawGrid(const GridConfig& vGridConfig) {
    auto* drawListPtr = ImGui::GetWindowDrawList();
    if (drawListPtr != nullptr) {
        const ImVec2& win_pos = ImGui::GetCursorScreenPos();
        const ImVec2& canvas_sz = ImGui::GetWindowSize();
        for (float x = fmodf(getScroll().x, vGridConfig.gridSize.x); x < canvas_sz.x; x += vGridConfig.gridSize.x) {
            drawListPtr->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, vGridConfig.gridColor);
        }
        for (float y = fmodf(getScroll().y, vGridConfig.gridSize.y); y < canvas_sz.y; y += vGridConfig.gridSize.y) {
            drawListPtr->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, vGridConfig.gridColor);
        }
        for (float x = fmodf(getScroll().x, vGridConfig.gridSize.x / vGridConfig.gridSubdivs.x); x < canvas_sz.x;
             x += vGridConfig.gridSize.x / vGridConfig.gridSubdivs.x) {
            drawListPtr->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, vGridConfig.subGridColor);
        }
        for (float y = fmodf(getScroll().y, vGridConfig.gridSize.y / vGridConfig.gridSubdivs.y); y < canvas_sz.y;
             y += vGridConfig.gridSize.y / vGridConfig.gridSubdivs.y) {
            drawListPtr->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, vGridConfig.subGridColor);
        }
    }
}

void ImCanvas::m_CopyIOEvents(ImGuiContext* src, ImGuiContext* dst, ImVec2 origin, float scale) {
    dst->InputEventsQueue = src->InputEventsTrail;
    for (ImGuiInputEvent& e : dst->InputEventsQueue) {
        if (e.Type == ImGuiInputEventType_MousePos) {
            e.MousePos.PosX = (e.MousePos.PosX - origin.x) / scale;
            e.MousePos.PosY = (e.MousePos.PosY - origin.y) / scale;
        }
    }
}

void ImCanvas::m_AppendDrawData(ImDrawList* src, ImVec2 origin, float scale) {
    // TODO optimize if vtx_start == 0 || if idx_start == 0
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const int vtx_start = dl->VtxBuffer.size();
    const int idx_start = dl->IdxBuffer.size();
    dl->VtxBuffer.resize(dl->VtxBuffer.size() + src->VtxBuffer.size());
    dl->IdxBuffer.resize(dl->IdxBuffer.size() + src->IdxBuffer.size());
    dl->CmdBuffer.reserve(dl->CmdBuffer.size() + src->CmdBuffer.size());
    dl->_VtxWritePtr = dl->VtxBuffer.Data + vtx_start;
    dl->_IdxWritePtr = dl->IdxBuffer.Data + idx_start;
    const ImDrawVert* vtx_read = src->VtxBuffer.Data;
    const ImDrawIdx* idx_read = src->IdxBuffer.Data;
    for (int i = 0, c = src->VtxBuffer.size(); i < c; ++i) {
        dl->_VtxWritePtr[i].uv = vtx_read[i].uv;
        dl->_VtxWritePtr[i].col = vtx_read[i].col;
        dl->_VtxWritePtr[i].pos = vtx_read[i].pos * scale + origin;
    }
    for (int i = 0, c = src->IdxBuffer.size(); i < c; ++i) {
        dl->_IdxWritePtr[i] = idx_read[i] + (ImDrawIdx)vtx_start;
    }
    for (auto cmd : src->CmdBuffer) {
        cmd.IdxOffset += idx_start;
        IM_ASSERT(cmd.VtxOffset == 0);
        cmd.ClipRect.x = cmd.ClipRect.x * scale + origin.x;
        cmd.ClipRect.y = cmd.ClipRect.y * scale + origin.y;
        cmd.ClipRect.z = cmd.ClipRect.z * scale + origin.x;
        cmd.ClipRect.w = cmd.ClipRect.w * scale + origin.y;
        dl->CmdBuffer.push_back(cmd);
    }

    dl->_VtxCurrentIdx += src->VtxBuffer.size();
    dl->_VtxWritePtr = dl->VtxBuffer.Data + dl->VtxBuffer.size();
    dl->_IdxWritePtr = dl->IdxBuffer.Data + dl->IdxBuffer.size();
}
