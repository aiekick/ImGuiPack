#include <Graph/Manager/NodeManager.h>

#include <Graph/Nodes/BluePrintNode.h>


bool NodeManager::init() {
    m_nodeEditor.rightClickPopUpContent([this](ImFlow::BaseNode* /*vNodePtr*/) {
        if (ImGui::MenuItem("Blueprint")) {
            m_nodeEditor.addNode<BluePrintNode>(ImGui::GetMousePos());
        }
    });
    return true;
}

void NodeManager::unit() {}
void NodeManager::drawGraph() { 
    m_nodeEditor.update(); }