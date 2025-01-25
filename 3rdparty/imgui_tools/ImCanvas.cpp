#include "ImCanvas.h"

static inline ImVec2 ImSelectPositive(const ImVec2& lhs, const ImVec2& rhs) {
    return ImVec2(lhs.x > 0.0f ? lhs.x : rhs.x, lhs.y > 0.0f ? lhs.y : rhs.y);
}

ImCanvas::ImCanvas() = default;
ImCanvas::~ImCanvas() = default;

const ImCanvas::Config& ImCanvas::getConfig() {
    return m_config;
}

ImCanvas::Config& ImCanvas::getConfigRef() {
    return m_config;
}

bool ImCanvas::begin(const char* id, const ImVec2& size) {
    return begin(ImGui::GetID(id), size);
}

bool ImCanvas::begin(ImGuiID id, const ImVec2& size) {
    IM_ASSERT(m_inBeginEnd == false);

    m_widgetPosition = ImGui::GetCursorScreenPos();
    m_widgetSize = ImSelectPositive(size, ImGui::GetContentRegionAvail());
    m_widgetRect = ImRect(m_widgetPosition, m_widgetPosition + m_widgetSize);
    m_drawList = ImGui::GetWindowDrawList();

    m_updateViewTransformPosition();

    if (ImGui::IsClippedEx(m_widgetRect, id))
        return false;

    // Save current channel, so we can assert when user
    // call canvas API with different one.
    m_expectedChannel = m_drawList->_Splitter._Current;

    // #debug: Canvas content.
    // m_drawList->AddRectFilled(m_startPos, m_startPos + m_CurrentSize, IM_COL32(0, 0, 0, 64));
    // m_drawList->AddRect(m_widgetRect.Min, m_widgetRect.Max, IM_COL32(255, 0, 255, 64));

    ImGui::SetCursorScreenPos(ImVec2(0.0f, 0.0f));

    m_saveInputState();
    m_saveViewportState();

    // Record cursor max to prevent scrollbars from appearing.
    m_windowCursorMaxBackup = ImGui::GetCurrentWindow()->DC.CursorMaxPos;

    m_enterLocalSpace();

    ImGui::SetNextItemAllowOverlap();

    // Emit dummy widget matching bounds of the canvas.
    ImGui::SetCursorScreenPos(m_viewRect.Min);
    ImGui::Dummy(m_viewRect.GetSize());

    ImGui::SetCursorScreenPos(ImVec2(0.0f, 0.0f));

    m_inBeginEnd = true;

    m_manageActions();

    return true;
}

void ImCanvas::drawScales() {
    ImVec2 subStepSize;
    if (m_config.gridSubdivs.x != 0.0f) {
        subStepSize.x = m_config.gridSize.x / m_config.gridSubdivs.x;
    }
    if (m_config.gridSubdivs.y != 0.0f) {
        subStepSize.y = m_config.gridSize.y / m_config.gridSubdivs.y;
    }
    if (m_viewRect.Max.x > 0.0f) {
        m_drawScale(ImVec2(0.0f, 0.0f), ImVec2(m_viewRect.Max.x, 0.0f), m_config.gridSize.x, subStepSize.x, m_config.scalesLabelAlignement, 1.0f);
    }
    if (m_viewRect.Min.x < 0.0f) {
        m_drawScale(ImVec2(0.0f, 0.0f), ImVec2(m_viewRect.Min.x, 0.0f), m_config.gridSize.x, subStepSize.x, m_config.scalesLabelAlignement, -1.0f);
    }
    if (m_viewRect.Max.y > 0.0f) {
        m_drawScale(ImVec2(0.0f, 0.0f), ImVec2(0.0f, m_viewRect.Max.y), m_config.gridSize.y, subStepSize.y, m_config.scalesLabelAlignement, 1.0f);
    }
    if (m_viewRect.Min.y < 0.0f) {
        m_drawScale(ImVec2(0.0f, 0.0f), ImVec2(0.0f, m_viewRect.Min.y), m_config.gridSize.y, subStepSize.y, m_config.scalesLabelAlignement, -1.0f);
    }
}

void ImCanvas::drawGrid() {
    auto* drawListPtr = ImGui::GetWindowDrawList();
    const ImVec2 offset = getView().getOrigin() * getView().getInvScale();
    if (drawListPtr != nullptr) {
        const auto win_pos = getViewRect().Min;
        const auto canvas_sz = getViewRect().GetSize();
        for (float x = fmodf(offset.x, m_config.gridSize.x); x < canvas_sz.x; x += m_config.gridSize.x) {
            drawListPtr->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, m_config.gridColor);
        }
        for (float y = fmodf(offset.y, m_config.gridSize.y); y < canvas_sz.y; y += m_config.gridSize.y) {
            drawListPtr->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, m_config.gridColor);
        }
        if (m_config.gridSubdivs.x != 0.0f && m_config.gridSubdivs.y != 0.0f) {
            const auto subStepSize = m_config.gridSize / m_config.gridSubdivs;
            for (float x = fmodf(offset.x, subStepSize.x); x < canvas_sz.x; x += subStepSize.x) {
                drawListPtr->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, m_config.subGridColor);
            }
            for (float y = fmodf(offset.y, subStepSize.y); y < canvas_sz.y; y += subStepSize.y) {
                drawListPtr->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, m_config.subGridColor);
            }
        }
    }
}

void ImCanvas::end() {
    // If you're here your call to begin() returned false,
    // or begin() wasn't called at all.
    IM_ASSERT(m_inBeginEnd == true);

    // If you're here, please make sure you do not interleave
    // channel splitter with canvas.
    // Always call canvas function with using same channel.
    IM_ASSERT(m_drawList->_Splitter._Current == m_expectedChannel);

    // auto& io = ImGui::GetIO();

    // Check: Unmatched calls to Suspend() / Resume(). Please check your code.
    IM_ASSERT(m_suspendCounter == 0);

    m_leaveLocalSpace();

    ImGui::GetCurrentWindow()->DC.CursorMaxPos = m_windowCursorMaxBackup;

    // Emit dummy widget matching bounds of the canvas.
    ImGui::SetCursorScreenPos(m_widgetPosition);
    ImGui::Dummy(m_widgetSize);

    // #debug: Rect around canvas. Content should be inside these bounds.
    // m_drawList->AddRect(m_widgetPosition - ImVec2(1.0f, 1.0f), m_widgetPosition + m_widgetSize + ImVec2(1.0f, 1.0f), IM_COL32(196, 0, 0, 255));

    m_inBeginEnd = false;
}

bool ImCanvas::isHovered()const {
    return ImGui::IsMouseHoveringRect(getViewRect().Min, getViewRect().Max);
}

void ImCanvas::setView(const ImVec2& origin, float scale) {
    setView(CanvasView(origin, scale));
}

void ImCanvas::setView(const CanvasView& view) {
    if (m_inBeginEnd) {
        m_leaveLocalSpace();
    }

    if (m_view.origin.x != view.origin.x || m_view.origin.y != view.origin.y) {
        m_view.origin = view.origin;
        m_updateViewTransformPosition();
    }

    if (m_view.scale != view.scale) {
        m_view.scale = view.scale;
        m_view.invScale = view.invScale;
    }

    if (m_inBeginEnd) {
        m_enterLocalSpace();
    }
}

void ImCanvas::centerView(const ImVec2& canvasPoint) {
    auto view = calcCenterView(canvasPoint);
    setView(view);
}

ImCanvas::CanvasView ImCanvas::calcCenterView(const ImVec2& canvasPoint) const {
    auto localcenter = screenToCanvas(m_widgetPosition + m_widgetSize * 0.5f);
    auto localOffset = canvasPoint - localcenter;
    auto offset = canvasToScreenV(localOffset);
    return CanvasView{m_view.origin - offset, m_view.scale};
}

void ImCanvas::centerView(const ImRect& canvasRect) {
    setView(calcCenterView(canvasRect));
}

ImCanvas::CanvasView ImCanvas::calcCenterView(const ImRect& canvasRect) const {
    auto canvasRectSize = canvasRect.GetSize();

    if (canvasRectSize.x <= 0.0f || canvasRectSize.y <= 0.0f) {
        return getView();
    }

    auto widgetAspectRatio = m_widgetSize.y > 0.0f ? m_widgetSize.x / m_widgetSize.y : 0.0f;
    auto canvasRectAspectRatio = canvasRectSize.y > 0.0f ? canvasRectSize.x / canvasRectSize.y : 0.0f;

    if (widgetAspectRatio <= 0.0f || canvasRectAspectRatio <= 0.0f) {
        return getView();
    }

    auto newOrigin = m_view.origin;
    auto newScale = m_view.scale;
    if (canvasRectAspectRatio > widgetAspectRatio) {
        // width span across view
        newScale = m_widgetSize.x / canvasRectSize.x;
        newOrigin = canvasRect.Min * -newScale;
        newOrigin.y += (m_widgetSize.y - canvasRectSize.y * newScale) * 0.5f;
    } else {
        // height span across view
        newScale = m_widgetSize.y / canvasRectSize.y;
        newOrigin = canvasRect.Min * -newScale;
        newOrigin.x += (m_widgetSize.x - canvasRectSize.x * newScale) * 0.5f;
    }

    return CanvasView{newOrigin, newScale};
}

void ImCanvas::suspend() {
    auto drawList = ImGui::GetWindowDrawList();
    auto lastChannel = drawList->_Splitter._Current;
    drawList->ChannelsSetCurrent(m_externalChannel);

    // If you're here, please make sure you do not interleave
    // channel splitter with canvas.
    // Always call canvas function with using same channel.
    IM_ASSERT(m_drawList->_Splitter._Current == m_expectedChannel);

    if (m_suspendCounter == 0) {
        m_leaveLocalSpace();
    }

    ++m_suspendCounter;

    drawList->ChannelsSetCurrent(lastChannel);
}

void ImCanvas::resume() {
    auto drawList = ImGui::GetWindowDrawList();
    auto lastChannel = drawList->_Splitter._Current;
    drawList->ChannelsSetCurrent(m_externalChannel);

    // If you're here, please make sure you do not interleave
    // channel splitter with canvas.
    // Always call canvas function with using same channel.
    IM_ASSERT(m_drawList->_Splitter._Current == m_expectedChannel);

    // Check: Number of calls to Resume() do not match calls to Suspend(). Please check your code.
    IM_ASSERT(m_suspendCounter > 0);
    if (--m_suspendCounter == 0)
        m_enterLocalSpace();

    drawList->ChannelsSetCurrent(lastChannel);
}

ImVec2 ImCanvas::canvasToScreen(const ImVec2& point) const {
    return point * m_view.scale + m_viewTransformPosition;
}

ImVec2 ImCanvas::canvasToScreen(const ImVec2& point, const CanvasView& view) const {
    return point * view.scale + view.origin + m_widgetPosition;
}

ImVec2 ImCanvas::canvasToScreenV(const ImVec2& vector) const {
    return vector * m_view.scale;
}

ImVec2 ImCanvas::canvasToScreenV(const ImVec2& vector, const CanvasView& view) const {
    return vector * view.scale;
}

ImVec2 ImCanvas::screenToCanvas(const ImVec2& point) const {
    return (point - m_viewTransformPosition) * m_view.invScale;
}

ImVec2 ImCanvas::screenToCanvas(const ImVec2& point, const CanvasView& view) const {
    return (point - view.origin - m_widgetPosition) * view.invScale;
}

ImVec2 ImCanvas::screenToCanvasV(const ImVec2& vector) const {
    return vector * m_view.invScale;
}

ImVec2 ImCanvas::screenToCanvasV(const ImVec2& vector, const CanvasView& view) const {
    return vector * view.invScale;
}

ImRect ImCanvas::calcViewRect(const CanvasView& view) const {
    ImRect result;
    result.Min = ImVec2(-view.origin.x, -view.origin.y) * view.invScale;
    result.Max = (m_widgetSize - view.origin) * view.invScale;
    return result;
}

void ImCanvas::m_resetActions() {
    m_backgroundClicked = false;
    m_backgroundDoubleClicked = false;
    m_requestContextMenuBackground = false;
}

void ImCanvas::m_manageActions() {
    m_resetActions();
    if (ImGui::IsWindowHovered() /* && ImGui::IsWindowFocused()*/) {
        m_manageZoom();
        m_manageDragging();
        m_manageContextMenus();
    }
}

void ImCanvas::m_manageContextMenus() {
    if (ImGui::IsMouseClicked(m_config.contextMenuButton) && !m_isDragging) {
        m_requestContextMenuBackground = true;
    }
}

void ImCanvas::m_manageZoom() {
    const auto& viewOrigin = getView().getOrigin();
    const auto& viewRect = getViewRect();
    auto viewScale = getView().getScale();
    const auto& io = ImGui::GetIO();
    auto mousePos = io.MousePos;
    auto steps = (int)io.MouseWheel;
    auto zoom_reseted = ImGui::IsKeyPressed(m_config.resetZoomKey);
    if (IS_FLOAT_DIFFERENT(steps, 0.0f) || zoom_reseted) {
        auto oldView = CanvasView(viewOrigin, viewScale);
        viewScale = ImMax(viewScale + steps * m_config.zoomFactor, m_config.zoomStep);
        auto newView = CanvasView(viewOrigin, viewScale);
        auto screenPos = canvasToScreen(mousePos, oldView);
        auto canvasPos = screenToCanvas(screenPos, newView);
        auto offset = (canvasPos - mousePos) * viewScale;
        auto targetScroll = viewOrigin + offset;
        if (zoom_reseted) {
            viewScale = 1.0f;
        }
        setView(targetScroll, viewScale);
    }
}

void ImCanvas::m_manageDragging() {
    const auto& viewOrigin = getView().getOrigin();
    const auto viewScale = getView().getScale();
    if ((m_isDragging || ImGui::IsItemHovered()) && ImGui::IsMouseDragging(m_config.draggingButton, 0.0f)) {
        if (!m_isDragging) {
            m_isDragging = true;
            m_drawStartPoint = viewOrigin;
        }
        setView(m_drawStartPoint + ImGui::GetMouseDragDelta(m_config.draggingButton, 0.0f) * viewScale, viewScale);
    } else if (m_isDragging) {
        m_isDragging = false;
    }
}

void ImCanvas::m_updateViewTransformPosition() {
    m_viewTransformPosition = m_view.origin + m_widgetPosition;
}

void ImCanvas::m_saveInputState() {
    auto& io = ImGui::GetIO();
    m_MousePosBackup = io.MousePos;
    m_MousePosPrevBackup = io.MousePosPrev;
    for (auto i = 0; i < IM_ARRAYSIZE(m_MouseClickedPosBackup); ++i) {
        m_MouseClickedPosBackup[i] = io.MouseClickedPos[i];
    }
}

void ImCanvas::m_restoreInputState() {
    auto& io = ImGui::GetIO();
    io.MousePos = m_MousePosBackup;
    io.MousePosPrev = m_MousePosPrevBackup;
    for (auto i = 0; i < IM_ARRAYSIZE(m_MouseClickedPosBackup); ++i) {
        io.MouseClickedPos[i] = m_MouseClickedPosBackup[i];
    }
}

void ImCanvas::m_saveViewportState() {
    auto window = ImGui::GetCurrentWindow();
    auto viewport = ImGui::GetWindowViewport();
    m_windowPosBackup = window->Pos;
    m_viewportPosBackup = viewport->Pos;
    m_viewportSizeBackup = viewport->Size;
    m_viewportWorkPosBackup = viewport->WorkPos;
    m_viewportWorkSizeBackup = viewport->WorkSize;
}

void ImCanvas::m_restoreViewportState() {
    auto window = ImGui::GetCurrentWindow();
    auto viewport = ImGui::GetWindowViewport();
    window->Pos = m_windowPosBackup;
    viewport->Pos = m_viewportPosBackup;
    viewport->Size = m_viewportSizeBackup;
    viewport->WorkPos = m_viewportWorkPosBackup;
    viewport->WorkSize = m_viewportWorkSizeBackup;
}

void ImCanvas::m_enterLocalSpace() {
    // Prepare ImDrawList for drawing in local coordinate system:
    //   - determine visible part of the canvas
    //   - start unique draw command
    //   - add clip rect matching canvas size
    //   - record current command index
    //   - record current vertex write index

    // Determine visible part of the canvas. Make it before
    // adding new command, to avoid round rip where command
    // is removed in PopClipRect() and added again next PushClipRect().
    ImGui::PushClipRect(m_widgetPosition, m_widgetPosition + m_widgetSize, true);
    auto clipped_clip_rect = m_drawList->_ClipRectStack.back();
    ImGui::PopClipRect();

    // Make sure we do not share draw command with anyone. We don't want to mess
    // with someones clip rectangle.

    // #FIXME:
    //     This condition is not enough to avoid when user choose
    //     to use channel splitter.
    //
    //     To deal with Suspend()/Resume() calls empty draw command
    //     is always added then splitter is active. Otherwise
    //     channel merger will collapse our draw command one with
    //     different clip rectangle.
    //
    //     More investigation is needed. To get to the bottom of this.
    // if ((!m_drawList->CmdBuffer.empty() && m_drawList->CmdBuffer.back().ElemCount > 0) || m_drawList->_Splitter._Count > 1)
    //    m_drawList->AddCallback(&ImCanvasDetails::SentinelDrawCallback, nullptr);

    m_drawListCommadBufferSize = ImMax(m_drawList->CmdBuffer.Size - 1, 0);
    m_drawListStartVertexIndex = m_drawList->_VtxCurrentIdx + m_drawList->_CmdHeader.VtxOffset;

    auto window = ImGui::GetCurrentWindow();
    window->Pos = ImVec2(0.0f, 0.0f);

    auto viewport_min = m_viewportPosBackup;
    auto viewport_max = m_viewportPosBackup + m_viewportSizeBackup;

    viewport_min.x = (viewport_min.x - m_viewTransformPosition.x) * m_view.invScale;
    viewport_min.y = (viewport_min.y - m_viewTransformPosition.y) * m_view.invScale;
    viewport_max.x = (viewport_max.x - m_viewTransformPosition.x) * m_view.invScale;
    viewport_max.y = (viewport_max.y - m_viewTransformPosition.y) * m_view.invScale;

    auto viewport = ImGui::GetWindowViewport();
    viewport->Pos = viewport_min;
    viewport->Size = viewport_max - viewport_min;

    viewport->WorkPos = m_viewportWorkPosBackup * m_view.invScale;
    viewport->WorkSize = m_viewportWorkSizeBackup * m_view.invScale;

    // Clip rectangle in parent canvas space and move it to local space.
    clipped_clip_rect.x = (clipped_clip_rect.x - m_viewTransformPosition.x) * m_view.invScale;
    clipped_clip_rect.y = (clipped_clip_rect.y - m_viewTransformPosition.y) * m_view.invScale;
    clipped_clip_rect.z = (clipped_clip_rect.z - m_viewTransformPosition.x) * m_view.invScale;
    clipped_clip_rect.w = (clipped_clip_rect.w - m_viewTransformPosition.y) * m_view.invScale;
    ImGui::PushClipRect(ImVec2(clipped_clip_rect.x, clipped_clip_rect.y), ImVec2(clipped_clip_rect.z, clipped_clip_rect.w), false);

    // Transform mouse position to local space.
    auto& io = ImGui::GetIO();
    io.MousePos = (m_MousePosBackup - m_viewTransformPosition) * m_view.invScale;
    io.MousePosPrev = (m_MousePosPrevBackup - m_viewTransformPosition) * m_view.invScale;
    for (auto i = 0; i < IM_ARRAYSIZE(m_MouseClickedPosBackup); ++i) {
        io.MouseClickedPos[i] = (m_MouseClickedPosBackup[i] - m_viewTransformPosition) * m_view.invScale;
    }

    m_viewRect = calcViewRect(m_view);

    auto& fringeScale = m_drawList->_FringeScale;
    m_lastFringeScale = fringeScale;
    fringeScale *= m_view.invScale;
}

void ImCanvas::m_leaveLocalSpace() {
    IM_ASSERT(m_drawList->_Splitter._Current == m_expectedChannel);

    // Move vertices to screen space.
    auto vertex = m_drawList->VtxBuffer.Data + m_drawListStartVertexIndex;
    auto vertexEnd = m_drawList->VtxBuffer.Data + m_drawList->_VtxCurrentIdx + m_drawList->_CmdHeader.VtxOffset;

    // If canvas view is not scaled take a faster path.
    if (m_view.scale != 1.0f) {
        while (vertex < vertexEnd) {
            vertex->pos.x = vertex->pos.x * m_view.scale + m_viewTransformPosition.x;
            vertex->pos.y = vertex->pos.y * m_view.scale + m_viewTransformPosition.y;
            ++vertex;
        }

        // Move clip rectangles to screen space.
        for (int i = m_drawListCommadBufferSize; i < m_drawList->CmdBuffer.size(); ++i) {
            auto& command = m_drawList->CmdBuffer[i];
            command.ClipRect.x = command.ClipRect.x * m_view.scale + m_viewTransformPosition.x;
            command.ClipRect.y = command.ClipRect.y * m_view.scale + m_viewTransformPosition.y;
            command.ClipRect.z = command.ClipRect.z * m_view.scale + m_viewTransformPosition.x;
            command.ClipRect.w = command.ClipRect.w * m_view.scale + m_viewTransformPosition.y;
        }
    } else {
        while (vertex < vertexEnd) {
            vertex->pos.x = vertex->pos.x + m_viewTransformPosition.x;
            vertex->pos.y = vertex->pos.y + m_viewTransformPosition.y;
            ++vertex;
        }

        // Move clip rectangles to screen space.
        for (int i = m_drawListCommadBufferSize; i < m_drawList->CmdBuffer.size(); ++i) {
            auto& command = m_drawList->CmdBuffer[i];
            command.ClipRect.x = command.ClipRect.x + m_viewTransformPosition.x;
            command.ClipRect.y = command.ClipRect.y + m_viewTransformPosition.y;
            command.ClipRect.z = command.ClipRect.z + m_viewTransformPosition.x;
            command.ClipRect.w = command.ClipRect.w + m_viewTransformPosition.y;
        }
    }

    auto& fringeScale = m_drawList->_FringeScale;
    fringeScale = m_lastFringeScale;

    // And pop \o/
    ImGui::PopClipRect();

    m_restoreInputState();
    m_restoreViewportState();
}

void ImCanvas::m_drawScale(const ImVec2& from, const ImVec2& to, float majorUnit, float minorUnit, float labelAlignment, float sign) {
    auto drawList = ImGui::GetWindowDrawList();
    auto direction = (to - from) * ImInvLength(to - from, 0.0f);
    auto normal = ImVec2(-direction.y, direction.x);
    auto distance = sqrtf(ImLengthSqr(to - from));

    if (ImDot(direction, direction) < FLT_EPSILON) {
        return;
    }
    
    auto minorSize = 5.0f;
    auto majorSize = 10.0f;
    auto labelDistance = 8.0f;

    drawList->AddLine(from, to, IM_COL32(255, 255, 255, 255));

    auto p = from;
    if (minorUnit != 0.0f) {
        for (auto d = 0.0f; d <= distance; d += minorUnit, p += direction * minorUnit) {
            drawList->AddLine(p - normal * minorSize, p + normal * minorSize, IM_COL32(255, 255, 255, 255));
        }
    }
    
    for (auto d = 0.0f; d <= distance + majorUnit; d += majorUnit) {
        p = from + direction * d;

        drawList->AddLine(p - normal * majorSize, p + normal * majorSize, IM_COL32(255, 255, 255, 255));

        if (d == 0.0f) {
            continue;
        }
        
        char label[16];
        snprintf(label, 15, "%g", d * sign);
        auto labelSize = ImGui::CalcTextSize(label);

        auto labelPosition = p + ImVec2(fabsf(normal.x), fabsf(normal.y)) * labelDistance;
        auto labelAlignedSize = ImDot(labelSize, direction);
        labelPosition += direction * (-labelAlignedSize + labelAlignment * labelAlignedSize * 2.0f);
        labelPosition = ImFloor(labelPosition + ImVec2(0.5f, 0.5f));

        drawList->AddText(labelPosition, IM_COL32(255, 255, 255, 255), label);
    }
}
