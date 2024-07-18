#pragma once

#include <sstream>
#include <unordered_map>

#include "../Parser/Nodes/Nodes.h"

using namespace Radium::Parser::Nodes;

namespace Radium::Generator
{
    class Generator {
    public:
        explicit Generator(NodeRoot nodeRoot);

        std::string generate();
    private:
        NodeRoot m_nodeRoot;

        std::stringstream m_sstream;

        std::unordered_map<std::string, size_t> m_identifierStackPositions;

        void mov(const std::string& reg, const std::string& val);

        size_t m_stackSizeBytes;
        void push(const std::string& reg, size_t sizeBytes);
        void pop(const std::string& reg, size_t sizeBytes);

        void generateStatement(const NodeStatement& statement);
        void generateStatementExit(const NodeStatementExit& statement);
        void generateStatementLet(const NodeStatementLet& statement);

        void generateExpression(const NodeExpression& expression, const std::string& desinationRegister);
        void generateExpressionIntLit(const NodeExpressionIntLit& expression, const std::string& destinationRegister);
        void generateExpressionIdentifier(const NodeExpressionIdentifer& expression, const std::string& destinationRegister);
    };

}
