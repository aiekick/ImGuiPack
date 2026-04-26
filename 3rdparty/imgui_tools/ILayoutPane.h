/*
Copyright 2022-2024 Stephane Cuillerdier (aka aiekick)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <cstdint>
#include <memory>
#include <string>

typedef int64_t LayoutPaneFlag; // int64_t give 64 Panes max.
typedef std::string LayoutPaneName;

struct ImVec2;
struct ImRect;
struct ImGuiContext;

struct ContextDatas {
    uint32_t currentFrame{0U};
    ImGuiContext* pImGuiContext{nullptr};
};

class ILayoutPane {
private: 
    LayoutPaneName paneName;
    LayoutPaneFlag paneFlag = -1;

public:
    virtual bool Init() = 0; // return false if the init was failed
    virtual void Unit() = 0;

    // the return, is a user side use case here
    virtual bool DrawPanes(bool* apOpened, ContextDatas& aContext, void* apUserDatas) = 0;
    virtual bool DrawWidgets(ContextDatas& aContext, void*apvUserDatas) = 0;
    virtual bool DrawOverlays(const ImRect& aRect, ContextDatas& aContext, void* apUserDatas) = 0;
    virtual bool DrawDialogsAndPopups(const ImRect& aRect, ContextDatas& aContext, void* apUserDatas) = 0;

    // if for any reason the pane must be hidden temporary, the user can control this here
    virtual bool CanBeDisplayed() = 0;

public:
    void SetName(const LayoutPaneName& aName) {
        paneName = aName;
    }
    const LayoutPaneName& GetName() const {
        return paneName;
    }
    void SetFlag(const LayoutPaneFlag& aFlag) {
        if (paneFlag < 0) {  // ensure than this can be done only one time
            paneFlag = aFlag;
        }
    }
    const LayoutPaneFlag& GetFlag() const {
        return paneFlag;
    }
};

typedef std::weak_ptr<ILayoutPane> ILayoutPaneWeak;
