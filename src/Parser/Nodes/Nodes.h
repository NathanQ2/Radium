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

    struct NodeAtom
    {
        std::variant<
            NodeExpressionIntLit*,
            NodeExpressionIdentifier*
        > variant;
    };

    struct NodeExpression
    {
        std::variant<
            NodeAtom*,
            NodeExpressionAdd*
        > variant;
    };

    struct NodeStatementExit
    {
        NodeExpression* exitCode;
    };

    struct NodeStatementLet
    {
        std::string identifier;
        NodeExpression* value;
    };

    struct NodeStatement
    {
        std::variant<
            NodeStatementLet*,
            NodeStatementExit*
        > variant;
    };

    struct NodeRoot
    {
        std::vector<NodeStatement> statements;
    };
}