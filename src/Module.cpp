#include "Module.h"

Module::Module(std::string path, NodeRoot root) : m_path(path), m_rootNode(root)
{
}

std::string_view Module::getPath() const
{
    return m_path;
}

const NodeRoot& Module::getRoot() const
{
    return m_rootNode;
}
