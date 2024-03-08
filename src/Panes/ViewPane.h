#pragma once

#include <ImGuiPack.h>
#include <cstdint>
#include <memory>
#include <string>

class ProjectFile;
class ViewPane : public AbstractPane {
public:
    bool Init() override;
    void Unit() override;
    bool DrawWidgets(const uint32_t& vCurrentFrame, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) override;
    bool DrawOverlays(
        const uint32_t& vCurrentFrame, const ImRect& vRect, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) override;
    bool DrawPanes(
        const uint32_t& vCurrentFrame, PaneFlags& vInOutPaneShown, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) override;
    bool DrawDialogsAndPopups(
        const uint32_t& vCurrentFrame, const ImVec2& vMaxSize, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) override;

public:  // singleton
    static std::shared_ptr<ViewPane> Instance() {
        static std::shared_ptr<ViewPane> _instance = std::make_shared<ViewPane>();
        return _instance;
    }

public:
    ViewPane();                              // Prevent construction
    ViewPane(const ViewPane&) = default;  // Prevent construction by copying
    ViewPane& operator=(const ViewPane&) {
        return *this;
    };                       // Prevent assignment
    virtual ~ViewPane();  // Prevent unwanted destruction};
};
