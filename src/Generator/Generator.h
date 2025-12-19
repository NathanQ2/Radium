#pragma once

#include <sstream>
#include <unordered_map>

#include "../CompilationUnit.h"
#include "../Parser/Nodes/Nodes.h"

using namespace Radium;

namespace Radium
{
    class Generator {
    public:
        explicit Generator(const CompilationUnit& cu);

        std::string generate();
    private:
        const CompilationUnit& m_cu;
        std::unordered_map<std::string_view, bool> m_generatedModules;

        std::stringstream m_ss;

        std::unordered_map<std::string, size_t> m_identifierStackPositions;
        std::unordered_map<std::string, bool> m_registers;
        size_t m_stackSizeBytes;

        int m_labels = 1;

        std::string nextLabel()
        {
            std::stringstream ss;
            ss << ".L" << m_labels;
            
            m_labels++;
            
            return ss.str();
        }

        void mov(const std::string& dest, const std::string& val);

        void push(const std::string& reg, size_t sizeBytes);
        void pop(const std::string& reg, size_t sizeBytes);

        std::string reserveRegister();
        void freeRegister(const std::string& reg);

        void generateModule(const Module& module);
        
        void generateFunction(const NodeFunctionDecl* func);
        void generateBlock(const NodeBlock* block);
        void generateStatement(const NodeStatement* statement);

        void generateVarDecl(const NodeVarDecl* varDecl);
        void generateExpressionStatement(const NodeExpressionStatement* statement);
        void generateReturnStatement(const NodeReturnStatement* ret);
        void generateIfStatement(const NodeIfStatement* ifStmt);

        void generateExpression(const NodeExpression* expr, const std::string& dest);
        void generateAssignment(const NodeAssignment* assignment, const std::string& dest);
        void generateAssignment(std::pair<NodeIdentifier*, NodeExpression*> assignment, const std::string& dest);
        void generateAdditive(const NodeAdditive* additive, const std::string& dest);
        void generateMultiplicative(const NodeMultiplicative* multi, const std::string& dest);
        void generatePrimary(const NodePrimary* primary, const std::string& dest);
        void generateNumber(const NodeNumber* number, const std::string& dest);
        void generateIdentifier(const NodeIdentifier* ident, const std::string& dest);
        void generateCall(const NodeCall* call, const std::string& dest);
        void generateFunctionExitInline(const NodeCall* call);
    };

}
