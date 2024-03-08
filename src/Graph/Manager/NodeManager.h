#pragma once

#include <ImGuiPack.h>

class NodeManager {
private:
    ImFlow::ImNodeFlow m_nodeEditor;

public:
    bool init();
    void unit();
    void drawGraph();

public:
    static NodeManager* Instance() {
        static NodeManager _instance;
        return &_instance;
    }
};
