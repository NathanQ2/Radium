#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <variant>

namespace Radium::Parser::Nodes
{
    struct NodeExpressionIntLit
    {
        std::string value;
    };

    // struct NodeExpressionIdentifer

    struct NodeExpression
    {
        std::variant<NodeExpressionIntLit> variant;
    };


    struct NodeStatementExit
    {
        NodeExpression expression;
    };

    struct NodeStatement
    {
        std::variant<NodeStatementExit> variant;
    };

    struct NodeRoot
    {
        std::vector<NodeStatement> statements;
    };
}