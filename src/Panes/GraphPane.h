#pragma once

#include <ImGuiPack.h>
#include <cstdint>
#include <memory>
#include <string>

class ProjectFile;
class GraphPane : public AbstractPane {
public:
    bool Init() override;
    void Unit() override;
    bool DrawWidgets(const uint32_t& vCurrentFrame, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) override;
    bool DrawOverlays(const uint32_t& vCurrentFrame, const ImRect& vRect, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) override;
    bool DrawPanes(const uint32_t& vCurrentFrame, PaneFlags& vInOutPaneShown, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) override;
    bool DrawDialogsAndPopups(const uint32_t& vCurrentFrame, const ImVec2& vMaxSize, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) override;

public:  // singleton
    static std::shared_ptr<GraphPane> Instance() {
        static std::shared_ptr<GraphPane> _instance = std::make_shared<GraphPane>();
        return _instance;
    }

public:
    GraphPane();                                               // Prevent construction
    GraphPane(const GraphPane&) = default;                     // Prevent construction by copying
    GraphPane& operator=(const GraphPane&) { return *this; };  // Prevent assignment
    virtual ~GraphPane();                                      // Prevent unwanted destruction};
};
