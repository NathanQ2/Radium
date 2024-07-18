#pragma once

#include <sstream>

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

        void generateStatement(const NodeStatement& statement);
        void generateStatementExit(const NodeStatementExit& statement);
        void generateExpression(const NodeExpression& expression, const std::string& desinationRegister);
        void generateExpressionIntLit(const NodeExpressionIntLit& expression, const std::string& destinationRegister);
    };

}
