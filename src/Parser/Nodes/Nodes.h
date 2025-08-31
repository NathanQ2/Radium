#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <variant>

namespace Radium
{
    struct NodeExpression;
    
    struct NodeNumber
    {
        std::string value;
    };
    
    struct NodeIdentifier
    {
        std::string value;
    };

    struct NodeCall
    {
        NodeIdentifier* identifier;
    };

    struct NodePrimary
    {
        std::variant<
            NodeNumber*,
            NodeIdentifier*,
            NodeCall*,
            NodeExpression*> value;
    };

    struct NodeMultiplicative
    {
        NodePrimary* primary;
    };

    struct NodeAdditive
    {
        NodeMultiplicative* left;
        NodeMultiplicative* right;
    };


    struct NodeAssignment
    {
        std::variant<
            std::pair<NodeIdentifier*, NodeExpression*>,
            NodeAdditive*
        > assignment;
    };

    struct NodeExpression
    {
        std::variant<NodeAssignment*, NodeAdditive*> expr;
    };

    struct NodeReturnStatement
    {
        NodeExpression* expression;
    };

    struct NodeExpressionStatement
    {
        NodeExpression* expression;
    };

    struct NodeVarDecl
    {
        NodeAssignment* assignment;
    };

    struct NodeStatement;

    struct NodeBlock
    {
        std::vector<NodeStatement*> statements;
    };

    struct NodeStatement
    {
        std::variant<
            NodeVarDecl*,
            NodeExpressionStatement*,
            NodeReturnStatement*,
            NodeBlock*> stmt;
    };

    struct NodeParam
    {
        std::variant<NodeIdentifier*, NodeNumber*> value;
    };

    struct NodeParamList
    {
        std::vector<NodeParam*> params;
    };
    
    struct NodeFunctionDecl
    {
        NodeIdentifier* identifier;
        NodeBlock* block;
    };
    
    struct NodeProgram
    {
        std::vector<NodeFunctionDecl*> functions;
    };
}
