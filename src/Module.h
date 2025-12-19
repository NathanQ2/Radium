#pragma once
#include "Parser/Nodes/Nodes.h"

using namespace Radium;

class Module
{
public:
    Module(std::string_view path, NodeRoot root);
    
    std::string_view getPath() const;
    
    const NodeRoot& getRoot() const;
private:
    std::string_view m_path;
    NodeRoot m_rootNode;
};
