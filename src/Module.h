#pragma once
#include "Parser/Nodes/Nodes.h"

using namespace Radium;

class Module
{
public:
    Module(std::string path, NodeRoot root);
    
    [[nodiscard]] std::string_view getPath() const;
    [[nodiscard]] const NodeRoot& getRoot() const;
private:
    std::string m_path;
    NodeRoot m_rootNode;
};
