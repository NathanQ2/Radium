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

    struct NodeExpressionIdentifer
    {
        std::string value;
    };

    struct NodeExpression
    {
        std::variant<NodeExpressionIntLit, NodeExpressionIdentifer> variant;
    };


    struct NodeStatementExit
    {
        NodeExpression expression;
    };

    struct NodeStatementLet
    {
        std::string identifier;
        NodeExpression expression;
    };

    struct NodeStatement
    {
        std::variant<NodeStatementLet, NodeStatementExit> variant;
    };

    struct NodeRoot
    {
        std::vector<NodeStatement> statements;
    };
}