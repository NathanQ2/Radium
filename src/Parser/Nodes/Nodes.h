#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <variant>

namespace Radium
{
    struct NodeExpression;
    struct NodeBlock;
    
    struct NodeNumber
    {
        std::string value;
    };
    
    struct NodeIdentifier
    {
        std::string value;
    };

    struct NodeArgList
    {
        std::vector<NodeExpression*> args;
    };

    struct NodeCall
    {
        NodeIdentifier* identifier;
        NodeArgList* argList;
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
        bool isSubtraction = false;
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


    struct NodeIfStatement
    {
        NodeExpression* expr;
        NodeBlock* block;
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
            NodeBlock*,
            NodeIfStatement*> stmt;
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
    
    struct NodeModuleDecl
    {
        std::string modPath;
    };
    
    struct NodeModuleInclude
    {
        std::string modPath;
    };
    
    struct NodeRoot
    {
        NodeModuleDecl* modDecl;
        
        std::vector<NodeModuleInclude*> includes;
        std::vector<NodeFunctionDecl*> functions;
    };
}
