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
        std::unique_ptr<NodeExpression> lhs;
        std::unique_ptr<NodeExpression> rhs;
    };

    struct NodeExpression
    {
        std::variant<
            std::unique_ptr<NodeExpressionIntLit>,
            std::unique_ptr<NodeExpressionIdentifier>,
            std::unique_ptr<NodeExpressionAdd>
        > variant;
    };

    struct NodeStatementExit
    {
        std::unique_ptr<NodeExpression> expression;
    };

    struct NodeStatementLet
    {
        std::string identifier;
        std::unique_ptr<NodeExpression> expression;
    };

    struct NodeStatement
    {
        std::variant<
            std::unique_ptr<NodeStatementLet>,
            std::unique_ptr<NodeStatementExit>
        > variant;
    };

    struct NodeRoot
    {
        std::vector<NodeStatement> statements;
    };
}