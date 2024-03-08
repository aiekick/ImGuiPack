#include <Graph/Nodes/BluePrintNode.h>

using namespace ImFlow;

BluePrintNode::BluePrintNode() {
    setTitle("Shader");
    setStyle(NodeStyle::brown());
    addIN<int>("in",0);
    addOUT<int>("out")->behaviour([this]() { return 0; });
}

BluePrintNode ::~BluePrintNode() {}
