#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <variant>

namespace Radium
{
    struct NodeExpressionIntLit
    {
        std::string value;
    };

    struct NodeExpressionIdentifier
    {
        std::string value;
    };

    struct NodeExpression;

    struct NodeExpressionAdd
    {
        NodeExpression* lhs;
        NodeExpression* rhs;
    };

    struct NodeExpression
    {
        std::variant<
            NodeExpressionIntLit,
            NodeExpressionIdentifier,
            NodeExpressionAdd
        > variant;
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