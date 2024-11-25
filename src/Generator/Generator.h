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

        std::stringstream m_sstream;

        std::unordered_map<std::string, size_t> m_identifierStackPositions;
        std::unordered_map<std::string, bool> m_registers;

        void mov(const std::string& reg, const std::string& val);

        size_t m_stackSizeBytes;
        void push(const std::string& reg, size_t sizeBytes);
        void pop(const std::string& reg, size_t sizeBytes);

        std::string pushRegister();
        void popRegister(const std::string& reg);

        void generateStatement(const NodeStatement& statement);
        void generateStatementExit(std::unique_ptr<NodeStatementExit> statement);
        void generateStatementLet(std::unique_ptr<NodeStatementLet> statement);

        void generateExpression(std::unique_ptr<NodeExpression> expression, const std::string& desinationRegister);
        void generateExpressionIntLit(std::unique_ptr<NodeExpressionIntLit> expression, const std::string& destinationRegister);
        void generateExpressionIdentifier(std::unique_ptr<NodeExpressionIdentifier> expression, const std::string& destinationRegister);
        void generateExpressionAdd(std::unique_ptr<NodeExpressionAdd> expression, const std::string& destinationRegister, const std::string& tempRegister);
    };

}
