#include "CompilationUnit.h"

#include "Logger/Logger.h"

CompilationUnit::CompilationUnit()
{
}

void CompilationUnit::addModule(NodeRoot root)
{
    m_modules.emplace_back(root.modDecl->modPath, root);
}

const std::vector<Module>& CompilationUnit::getModules() const
{
    return m_modules;
}

const Module& CompilationUnit::getModuleByPath(std::string_view path) const
{
    const auto it = std::find_if(m_modules.begin(), m_modules.end(), [path](const Module& mod) { return path == mod.getPath(); });
    if (it == m_modules.end())
    {
        RA_ERROR("Could not find module by type!");
        
        exit(1);
    }
    
    return *it;
}
