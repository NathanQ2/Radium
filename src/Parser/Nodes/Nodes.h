#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <variant>

namespace Radium::Parser::Nodes
{
    struct TermIntLit
    {
        std::string value;
    };

    struct TermIdentifier
    {
        std::string value;
    };

    struct Term
    {
        std::variant<TermIntLit, TermIdentifier> variant;
    };

    struct NodeExpression;

    struct NodeExpressionAdd
    {
        NodeExpression* lhs;
        NodeExpression* rhs;
    };

    struct NodeExpressionMult
    {
        NodeExpression* lhs;
        NodeExpression* rhs;
    };

    struct NodeExpression
    {
        std::variant<
            Term,
            NodeExpressionAdd,
            NodeExpressionMult
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