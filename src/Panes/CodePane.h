#pragma once

#include <ImGuiPack.h>
#include <cstdint>
#include <memory>
#include <string>

class ProjectFile;
class CodePane : public AbstractPane {
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
    static std::shared_ptr<CodePane> Instance() {
        static std::shared_ptr<CodePane> _instance = std::make_shared<CodePane>();
        return _instance;
    }

public:
    CodePane();                              // Prevent construction
    CodePane(const CodePane&) = default;  // Prevent construction by copying
    CodePane& operator=(const CodePane&) {
        return *this;
    };                       // Prevent assignment
    virtual ~CodePane();  // Prevent unwanted destruction};
};
