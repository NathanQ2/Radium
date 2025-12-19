#pragma once
#include "Module.h"

class CompilationUnit
{
public:
    CompilationUnit();
    
    void addModule(NodeRoot root);
    
    const std::vector<Module>& getModules() const;
    const Module& getModuleByPath(std::string_view path) const;
private:
    std::vector<Module> m_modules;
};
