#pragma once

#include <sstream>
#include <unordered_map>

#include "../Parser/Nodes/Nodes.h"

using namespace Radium;

namespace Radium
{
    class Generator {
    public:
        explicit Generator(NodeRoot nodeRoot);

        std::string generate();
    private:
        NodeRoot m_nodeRoot;

        std::stringstream m_ss;

        std::unordered_map<std::string, size_t> m_identifierStackPositions;
        std::unordered_map<std::string, bool> m_registers;
        size_t m_stackSizeBytes;

        void mov(const std::string& dest, const std::string& val);

        void push(const std::string& reg, size_t sizeBytes);
        void pop(const std::string& reg, size_t sizeBytes);

        std::string reserveRegister();
        void freeRegister(const std::string& reg);

        void generateFunction(const NodeFunction& function);
        
        void generateStatement(const NodeStatement& statement);
        void generateStatementExit(NodeStatementExit* statement);
        void generateStatementLet(NodeStatementLet* statement);

        void generateExpression(NodeExpression* expression, const std::string& destinationRegister);
        void generateAtom(const NodeAtom* expression, const std::string& destinationRegister);
        void generateExpressionIntLit(const NodeExpressionIntLit* expression, const std::string& destinationRegister);
        void generateExpressionIdentifier(const NodeExpressionIdentifier* expression, const std::string& destinationRegister);
        void generateExpressionAdd(NodeExpressionAdd* expression, const std::string& destinationRegister, const std::string& tempRegister);
    };

}
